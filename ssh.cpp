#include <cstring>
#include "aes/aes.hpp"
#include "hmac/hmac.h"
#include "KeyGen.hpp"
#include "ssh.hpp"
#include <cassert>
#include <cmath>

static int genKeys(const std::string hmac_shared, const std::string aes_shared, ssh::Keys &keys) {
	hmac_shared = hmac_shared.substr(0, hmac::byte_length);
	memcpy(keys.hmac_key, hmac_shared.data(), hmac_shared.size());
	mpz_t mpz_aes_key, mpz_aes_iv;
	mpz_init (mpz_aes_key);
	mpz_init (mpz_aes_iv);
	constexpr size_t half_size = KeyGen::diffiekeysize / 2;
	std::string aes_key_str = aes_shared.substr(0, half_size);
	std::string aes_iv_str = aes_shared.substr(half_size);
	
	if (mpz_set_str(mpz_aes_key, aes_key_str.c_str(), KeyGen::base) == -1
			|| mpz_set_str(mpz_aes_iv, aes_iv_str.c_str(), KeyGen::base) == -1) {
		std::cerr << "Keys are of invalid base type" << std::endl;
		mpz_clear(mpz_aes_key);
		mpz_clear(mpz_aes_iv);
		return -1;
	}
	if (aes::fill_key(keys.aes_key, mpz_aes_key) == -1
			|| aes::fill_iv(keys.aes_iv, mpz_aes_iv) == -1) {
		mpz_clear(mpz_aes_key);
		mpz_clear(mpz_aes_iv);
		return -1;
	}
	mpz_clear(mpz_aes_key);
	mpz_clear(mpz_aes_iv);
	return 0;
}

int ssh::ClientDiffieKeys::genKeys(const char keyex_msg[SERVER_KEYEX_LEN], ssh::Keys &keys) {
	std::string hmac_half = std::string(keyex_msg, KeyGen::diffiekeyhalfsize);
	std::string aes_half
			= std::string(keyex_msg + KeyGen::diffiekeyhalfsize, KeyGen::diffiekeyhalfsize);
	std::string hmac_shared = KeyGen::getSharedKey(this->hmac_keys, hmac_half);
	std::string aes_shared = KeyGen::getSharedKey(this->aes_keys, aes_half);
	if (genKeys(hmac_shared, aes_shared, keys) == -1)
		return -1;
	return 0;
}

int ssh::ServerDiffieKeys::genKeys(ssh::Keys &keys) {
	if (genKeys(this->hmac_shared, this->aes_shared, keys) == -1)
		return -1;
	return 0;
}

ssh::RecvMsg::RecvMsg(const char msg[TOTAL_LEN], size_t recvlen, const Keys &keys) {
	if (recvlen != TOTAL_LEN) {
		error = "Bad message format: Invalid length";
		return;
	}
	std::string mac = std::string(msg, hmac::output_length);
	const char * ctxt = msg + hmac::output_length;
	char ptxt[AES_BUF_LEN];
	aes::cbc_decrypt(ctxt, ptxt, AES_BUF_LEN, keys.aes_iv, keys.aes_key);
	#ifndef NENCRYPT
	if (hmac::create_HMAC(std::string(ptxt, AES_BUF_LEN), keys.hmac_key).compare(mac) != 0) {
		error = "Corrupt message: Macs do not match";
		return;
	}
	#endif
	if ((unsigned int)ptxt[0] >= (unsigned int)MsgType::INVALID) {
		type = MsgType::INVALID;
		error = "Bad message format: Unknown message type";
		return;
	}
	type = (MsgType::Type) ptxt[0];
	uid = (unsigned char) ptxt[1];
	amt = 0;
	for (int i = 2; i < 2 + sizeof(uint64_t)/sizeof(char); i++) {
		amt <<= 8;
		amt |= (uint64_t) ptxt[i];
	}
}

ssh::SendMsg::SendMsg(MsgType::Type type, unsigned char uid, uint64_t amt, const Keys &keys) {
	char ptxt[AES_BUF_LEN] = {0};
	ptxt[0] = (char) type;
	ptxt[1] = (char) uid;
	constexpr uint64_t MASK = (1 << 8) - 1;
	for (int i = 0; i < sizeof(uint64_t)/sizeof(char); i++)
		ptxt[i + 2] = (char)((amt >> (56 - i * 8)) & MASK);
	char * ctxt = msg + hmac::output_length;
	std::cout << "\nhmac:::: " << std::string(keys.hmac_key, hmac::byte_length) << std::endl;
	aes::cbc_encrypt(ptxt, ctxt, AES_BUF_LEN, keys.aes_iv, keys.aes_key, keys.hmac_key);
	std::cout << "\nhmac:::: " << std::string(keys.hmac_key, hmac::byte_length) << std::endl;
	std::string mac = hmac::create_HMAC(std::string(ptxt, AES_BUF_LEN), keys.hmac_key);
	assert(mac.size() == hmac::output_length);
	memcpy(msg, mac.data(), hmac::output_length);
}

#ifdef TEST_SSH
int main() {
	std::cout << "ssh tests complete" << std::endl;
}
#endif
