#include <cstring>
#include "aes/aes.hpp"
#include "hmac/hmac.h"
#include "KeyGen.hpp"
#include "ssh.hpp"

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
		std::cout << ptxt[i] << std::endl;
		std::cout << (uint64_t) ptxt[i] << std::endl;
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
	aes::cbc_encrypt(ptxt, ctxt, AES_BUF_LEN, keys.aes_iv, keys.aes_key);
	std::string mac = hmac::create_HMAC(std::string(ptxt, AES_BUF_LEN), keys.hmac_key);
	std::cout << mac.size() << std::endl;
	assert(mac.size() == hmac::output_length);
	memcpy(msg, mac.c_str(), hmac::output_length);
}

#ifdef TEST_SSH
int main() {
	std::cout << "ssh tests complete" << std::endl;
}
#endif
