/* aes.cpp */
#include "aes.hpp"

class Poly {
	int32_t coefficients;
	public:
		Poly operator=(int32_t);
		Poly operator+(Poly);
		Poly operator*(Poly);
		Poly lcircsh(int);
};
struct State {
	static constexpr int NUM_ROWS = 4, NUM_COLS = aes::BLOCK_SIZE;
	Poly polys[NUM_ROWS][NUM_COLS];
};
