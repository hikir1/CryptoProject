
#ifndef _AES_H_
#define _AES_H_
#include <cstdint>

namespace aes {
	constexpr int KEY_SIZE = 4; // words
	void cbc_encrypt(const char * ptxt, char * ctxt, int len, const uint32_t key[KEY_SIZE]);
	void cbc_decrypt(const char * data, char * ctxt, int len, const uint32_t key[KEY_SIZE]);
}

#endif
