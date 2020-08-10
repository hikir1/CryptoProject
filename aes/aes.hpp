
#ifndef _AES_H_
#define _AES_H_
#include <cstdint>
#include <cstdlib>

namespace aes {
	constexpr int BLOCK_SIZE = 4; // words (32 bytes)
	constexpr int BLOCK_BYTES = BLOCK_SIZE * sizeof(uint32_t); // bytes
	constexpr int KEY_SIZE = 4; // words
	// ctxt must have at least BLOCK_BYTES extra space
	void cbc_encrypt(const char * ptxt, char * ctxt, size_t len,
			const uint32_t iv[BLOCK_SIZE], const uint32_t key[KEY_SIZE]);
	// len must be exact multiple of BLOCK_SIZE
	void cbc_decrypt(const char * ctxt, char * ptxt, size_t len,
			const uint32_t iv[BLOCK_SIZE], const uint32_t key[KEY_SIZE]);
}

#endif
