/* aes.cpp */

#include <cassert>
#include "aes.hpp"
#include "poly.cpp"
#ifndef NDEBUG
#include <iostream>
#endif

using namespace aes;

namespace {

	constexpr int BLOCK_SIZE = 4; // words (32 bytes)
	constexpr int KEY_SIZE = 4; // words
	constexpr int NUM_ROUNDS = 10;

	struct State {
		static constexpr int NUM_ROWS = 4, NUM_COLS = BLOCK_SIZE;
		Poly polys[NUM_ROWS * NUM_COLS];
		Poly * operator[](int idx) { 
			assert(idx < NUM_ROWS);
			return polys + idx * NUM_COLS;
		}
	};

	static_assert(State::NUM_ROWS * State::NUM_COLS * (NUM_ROUNDS + 1) % 4 == 0);
	constexpr int EXP_KEY_SIZE = State::NUM_ROWS * State::NUM_COLS * (NUM_ROUNDS + 1) / 4; // words

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
				for (int k = 0; k < State::NUM_ROWS; k++)
					temp[i][j] = temp[i][j] + s[i][k] * mults[(j + 3 + k) % State::NUM_ROWS];
			}
		}
		s = temp;
	}

	inline void mixColumns_F(State &s) {
		constexpr Poly mults[State::NUM_ROWS] = {0x03, 0x01, 0x01, 0x02};
		mixColumns(s, mults);
	}

	inline void mixColumns_R(State &s) {
		constexpr Poly mults[State::NUM_ROWS] = {0x0B, 0x0D, 0x09, 0x0E};
		mixColumns(s, mults);
	}

	void addRoundKey(State &s, const uint8_t subkey[State::NUM_ROWS * State::NUM_COLS]) {
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
			if (i % KEY_SIZE == 0) {
				// rotate
				rot = (temp << 8) | (temp >> 24);
				// subBytes
				constexpr int MASK = 0b1111'1111;
				temp = uint32_t(SBOX_F.vals[temp >> 24].raw()) << 24;
				temp |= uint32_t(SBOX_F.vals[(temp >> 16) & MASK].raw()) << 16;
				temp |= uint32_t(SBOX_F.vals[(temp >> 8) & MASK].raw()) << 8;
				temp |= uint32_t(SBOX_F.vals[temp & MASK].raw());
				// add constant
				assert(i/KEY_SIZE != 0);
				assert(i/KEY_SIZE <= 8);
				temp ^= 1 << i/KEY_SIZE + 23;
			}
			expanded[i] = expanded[i - KEY_SIZE] ^ temp;
		}
	}

}

void aes::encrypt(char * data, int len) {

}

void aes::decrypt(char * data, int len) {

}

#ifdef TEST_AES
#include <iostream>
int main() {
	Poly p = 0x42;
	assert(SBOX_R.vals[SBOX_F.vals[p.raw()].raw()].raw() == p.raw());
	std::cout << "aes test complete" << std::endl;
}
#endif
