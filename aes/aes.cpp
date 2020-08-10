/* aes.cpp */

#include <cassert>
#include "aes.hpp"
#include "poly.cpp"
#ifndef NDEBUG
#include <iostream>
#endif

using namespace aes;

namespace {

	constexpr int NUM_ROUNDS = 10;

	struct State {
		static constexpr int NUM_ROWS = 4, NUM_COLS = BLOCK_SIZE;
		Poly polys[NUM_ROWS * NUM_COLS];
		Poly * operator[](int idx) { 
			assert(idx < NUM_ROWS);
			return polys + idx * NUM_COLS;
		}
		State &operator^=(State &other) {
			for (int i = 0; i < NUM_ROWS * NUM_COLS; i++)
				polys[i] = polys[i] + other.polys[i];
			return *this;
		}
	};

	static_assert(State::NUM_ROWS * State::NUM_COLS * (NUM_ROUNDS + 1) % 4 == 0);
	using subkey_t = uint8_t[State::NUM_ROWS * State::NUM_COLS];
	// sizeof(subkey_t) * (NUM_ROUNDS + 1) / 4
	constexpr int EXP_KEY_SIZE = State::NUM_ROWS * State::NUM_COLS * (NUM_ROUNDS + 1) / 4; // words
	static_assert(sizeof(uint32_t[EXP_KEY_SIZE]) == sizeof(subkey_t[NUM_ROUNDS + 1]));

	// calculate SBOX at compile time (if not debugging, bc very slow)
	struct SBox_F {
		Poly vals[1 << 8];
		#ifdef NDEBUG
		constexpr
		#endif
		SBox_F() : vals() {
			for (int i = 0; i < 1 << 8; i++) {
				Poly inv = Poly((uint8_t)i).inv();
				vals[i] = inv + inv.circshl(1)
						+ inv.circshl(2) + inv.circshl(3)
						+ inv.circshl(4) + Poly(0x63);
			}
		}
	}
	#ifdef NDEBUG
	constexpr
	#else
	const
	#endif
	SBOX_F;

	// the reverse SBOX too
	struct SBox_R {
		Poly vals[1 << 8];
		#ifdef NDEBUG
		constexpr
		#endif
		SBox_R() : vals() {
			for (int i = 0; i < 1 << 8; i++) {
				Poly p = (uint8_t)i;
				p = p.circshl(1) + p.circshl(3) + p.circshl(6) + Poly(0x5);
				vals[i] = p.inv();
			}
		}
	}
	#ifdef NDEBUG
	constexpr
	#else
	const
	#endif
	SBOX_R;

	inline void subBytes_F(State &s) {
		for (Poly &p : s.polys)
			p = SBOX_F.vals[p.raw()];
	}

	inline void subBytes_R(State &s) {
		for (Poly &p : s.polys)
			p = SBOX_R.vals[p.raw()];
	}

	void shiftRows_F(State &s) {
		Poly temp;
		for (int i = 1; i < State::NUM_ROWS; i++) {
			temp = s[i][0];
			for (int j = 1; j < State::NUM_COLS; j++)
				s[i][j-1] = s[i][j];
			s[i][State::NUM_COLS - 1] = temp;
		}
	}

	void shiftRows_R(State &s) {
		Poly temp;
		for (int i = 1; i < State::NUM_ROWS; i++) {
			temp = s[i][State::NUM_COLS - 1];
			for (int j = State::NUM_COLS - 1; j > 0; j--)
				s[i][j] = s[i][j - 1];
			s[i][0] = temp;
		}
	}

	void mixColumns(State &s, const Poly mults[State::NUM_ROWS]) {
		State temp;
		for (int i = 0; i < State::NUM_COLS; i++) {
			for (int j = 0; j < State::NUM_ROWS; j++) {
				temp[i][j] = 0;
				for (int k = 0; k < State::NUM_ROWS; k++) {
					temp[i][j] = temp[i][j] + s[i][k] * 
							mults[(k - j + State::NUM_ROWS) % State::NUM_ROWS];
				}
			}
		}
		s = temp;
	}

	inline void mixColumns_F(State &s) {
		constexpr Poly mults[State::NUM_ROWS] = {0x02, 0x03, 0x01, 0x01};
		mixColumns(s, mults);
	}

	inline void mixColumns_R(State &s) {
		constexpr Poly mults[State::NUM_ROWS] = {0x0E, 0x0B, 0x0D, 0x09};
		mixColumns(s, mults);
	}

	void addRoundKey(State &s, const subkey_t subkey) {
		for (int i = 0; i < State::NUM_ROWS; i++)
			for (int j = 0; j < State::NUM_COLS; j++)
				s[i][j] = s[i][j] + Poly(subkey[i * j]);
	}

	void expandKey(const uint32_t key[KEY_SIZE], uint32_t expanded[EXP_KEY_SIZE]) {
		int i;
		for (i = 0; i < KEY_SIZE; i++)
			expanded[i] = key[i];
		uint32_t temp, rot;
		for (; i < EXP_KEY_SIZE; i++) {
			temp = expanded[i-1];
			constexpr int MASK = 0b1111'1111;
			if (i % KEY_SIZE == 0) {
				// rotate
				rot = (temp << 8) | (temp >> 24);
				// subBytes
				temp = uint32_t(SBOX_F.vals[rot >> 24].raw()) << 24;
				temp |= uint32_t(SBOX_F.vals[(rot >> 16) & MASK].raw()) << 16;
				temp |= uint32_t(SBOX_F.vals[(rot >> 8) & MASK].raw()) << 8;
				temp |= uint32_t(SBOX_F.vals[rot & MASK].raw());
				// add constant
				assert(i/KEY_SIZE != 0);
				assert(i/KEY_SIZE <= 15);
				if (i/KEY_SIZE <= 8)
					temp ^= 1 << (i/KEY_SIZE + 23);
				else
					temp ^= ((1 << (i/KEY_SIZE)) ^ Poly::MOD) << 23;
			}
			expanded[i] = expanded[i - KEY_SIZE] ^ temp;
		}
	}

	void encrypt(State &s, const subkey_t subkeys[NUM_ROUNDS + 1]) {
		for (int i = 0; i < NUM_ROUNDS; i++) {
			subBytes_F(s);
			shiftRows_F(s);
			mixColumns_F(s);
			addRoundKey(s, subkeys[i]);
		}
		subBytes_F(s);
		shiftRows_F(s);
		addRoundKey(s, subkeys[NUM_ROUNDS]);
	}

	void decrypt(State &s, const subkey_t subkeys[NUM_ROUNDS + 1]) {
		addRoundKey(s, subkeys[NUM_ROUNDS]);
		shiftRows_R(s);
		subBytes_R(s);
		for (int i = NUM_ROUNDS - 1; i >= 0; i--) {
			addRoundKey(s, subkeys[i]);
			mixColumns_R(s);
			shiftRows_R(s);
			subBytes_R(s);
		}
	}

}

#include <cstring>

// ctxt needs at least BLOCK_BYTES extra space (for padding)
void aes::cbc_encrypt(const char * ptxt, char * ctxt, size_t &len,  
		const uint32_t iv[BLOCK_SIZE], const uint32_t key[KEY_SIZE]) {
	subkey_t subkeys[NUM_ROUNDS + 1];
	expandKey(key, (uint32_t *)subkeys);
	memcpy(ctxt, ptxt, len);
	int padding_size = BLOCK_BYTES - len % BLOCK_BYTES;
	memset(ctxt + len, '\0', padding_size); // padding
	len += padding_size;
	State * blocks = (State *) ctxt;
	State * prev = (State *) iv;
	for (; len > 0; len -= BLOCK_BYTES) {
		*blocks ^= *prev;
		encrypt(*blocks, subkeys);
		prev = blocks;
		blocks++;
	}
}

// len must be multiple of BLOCK_BYTES
void aes::cbc_decrypt(const char * ctxt, char * ptxt, size_t len, 
		const uint32_t iv[BLOCK_SIZE], const uint32_t key[KEY_SIZE]) {
	assert(len % BLOCK_BYTES == 0);
	subkey_t subkeys[NUM_ROUNDS + 1];
	expandKey(key, (uint32_t *)subkeys);
	memcpy(ptxt, ctxt, len);
	State * blocks = (State *) ptxt;
	State * prevctxt = (State *) iv, * nextctxt = (State *) ctxt;
	for(; len > 0; len -= BLOCK_SIZE * 4) {
		decrypt(*blocks, subkeys);
		*blocks ^= *prevctxt;
		prevctxt = nextctxt;
		nextctxt++;
		blocks++;
	}

}

#ifdef TEST_AES
#include <iostream>
#include <cstdlib>
int main() {
#if 1
{ // test sboxes
	Poly p = 0x42;
	assert(SBOX_R.vals[SBOX_F.vals[p.raw()].raw()].raw() == p.raw());
}
#endif
#if 1
{ // test generic aes encrypt and decrypt
	char msg[] = "abcdefghijklmnop";
	size_t len = 16;
	uint32_t key[KEY_SIZE] = {1, 2, 3, 4};
	subkey_t subkeys[NUM_ROUNDS + 1];
	expandKey(key, (uint32_t *) subkeys);
	State s;
	memcpy(s.polys, msg, len);
	encrypt(s, subkeys);
	decrypt(s, subkeys);
	char ptxt[len + 1];
	ptxt[len] = 0;
	memcpy(ptxt, s.polys, len);
	assert(strcmp(msg, ptxt) == 0);
}
#endif
#if 1
{ // test cbc mode
	const char msgs[][64] = {
		"Hello world?", // well below block size
		"Hello world????", // block size - 1 (block size with null byte)
		"Hello world????a", // block size
		"Hello world????a bcdefg" // above block size
	};
	for (const char * msg : msgs) {
		size_t len = strlen(msg);
		char ctxt[len + BLOCK_BYTES];
		uint32_t iv[BLOCK_SIZE] = {0};
		uint32_t key[KEY_SIZE] = {0};
		aes::cbc_encrypt(msg, ctxt, len, iv, key);
		len += BLOCK_BYTES - len % BLOCK_BYTES;
		char ptxt[len + 1];
		aes::cbc_decrypt(ctxt, ptxt, len, iv, key);
		ptxt[len] = 0;
		assert(strcmp(msg, ptxt) == 0);
	}
}
#endif
	std::cout << "aes tests complete" << std::endl;
}
#endif
