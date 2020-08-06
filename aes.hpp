
#ifndef _AES_H_
#define _AES_H_

namespace aes {
	constexpr int BLOCK_SIZE = 4;
	using Block = int32_t[BLOCK_SIZE];
	void encrypt(Block);
	void decrypt(Block);
}

#endif
