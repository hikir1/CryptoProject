#include <cstdint>

// TODO: 
//	- make table for inverses

namespace aes {
	class Poly {
		public:
		const uint8_t coefs;
		Poly(uint8_t coefs): coefs(coefs) {}
	};

	// polynomial addition
	inline Poly operator+(Poly a, Poly b) {
		return a.coefs ^ b.coefs;
	}

	// same as subtraction
	inline Poly operator-(Poly a, Poly b) {
		return a + b;
	}

	// polynomial multiplication
	Poly operator*(Poly a, Poly b) {
		// product with overflow
		uint16_t prod = 0;
		for (int i = 0; i < 8; i++)
			prod ^= (a.coefs * ((b.coefs >> i) & 1)) << i;
		// modulous by long division
		constexpr uint16_t mod = 0x11B;
		int msig, mult;
		while (prod >> 8) {
			// index of most significant bit
			msig = 15;
			while (!((prod >> msig) & 1))
				msig--;
			// multiply divisor by multiple, then subtract from product
			mult = msig - 8;
			prod ^= mod << mult;
		}
		return Poly((uint8_t) prod);
	}

	// circular left shift
	inline Poly circshl(Poly p, int shamt) {
		return (p.coefs << shamt) | (p.coefs >> (8 - shamt));
	}

};

#ifdef TEST
#include <cstdio>
#include <cassert>
int main() {
	aes::Poly a = 0x57, b = 0x83;
	assert((a + b).coefs == 0xD4);
	assert((a * b).coefs == 0xC1);
	circshl(a, 2);
}
#endif
