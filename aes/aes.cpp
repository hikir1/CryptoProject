/* aes.cpp */
#include "aes.hpp"

constexpr int BLOCK_SIZE = 16; // Bytes

struct State {
	static constexpr int NUM_ROWS = 4, NUM_COLS = BLOCK_SIZE / 4;
	Poly polys[NUM_ROWS * NUM_COLS];
};

void subBytes(State * s) {

}

void shiftRows(State * s) {

}

void mixColumns(State * s) {

}

void addRoundKey(State * s) {

}
