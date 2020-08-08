#include <cstdint>
#ifndef NDEBUG
#include <iostream>
#endif

// TODO: 


namespace aes {

	class Poly {
		const uint8_t coefs;

		public:

		constexpr Poly(uint8_t coefs = 0): coefs(coefs) {}

		constexpr operator uint8_t() {
			return coefs;
		}

		// polynomial addition
		Poly operator+(Poly other) {
			return coefs ^ other.coefs;
		}

		// same as subtraction
		Poly operator-(Poly other) {
			return operator+(other);
		}

		// circular left shift
		Poly circshl(int shamt) {
			return (coefs << shamt) | (coefs >> (8 - shamt));
		}

		Poly operator*(Poly other);

		Poly inv();

		#ifndef NDEBUG
		friend std::ostream &operator<<(std::ostream &, Poly);
		#endif

	};
	// calculate inverses at compile time
	namespace {
		uint16_t MOD = 0x11B;
		uint8_t calcinv(uint16_t p) { // dif = p
			uint16_t prev0 = 0, prev1 = 1, cur = 0, mult = 0, dif = 0;
			uint16_t prevp = MOD;
			while (p) {
				for (mult = 1; prevp > p << (mult + 1); mult++);
				cur = mult*prev1 + prev0;
				prev0 = prev1;
				prev1 = cur;
				dif = prevp ^ (p << mult);
				prevp = p;
				p = dif;
				std::cout << p << std::endl;
			}
			return 0;
		}
		struct InvArray {
			uint8_t vals[1 << 8];
			InvArray() : vals() {
				for (uint16_t i = 1; i < 1 << 8; i++)
					vals[i] = calcinv(i);
			}
		} invarr;
	}

	inline Poly Poly::inv() {
		return invarr.vals[coefs];
	}

	#ifndef NDEBUG
	std::ostream &operator<<(std::ostream &out, Poly p) {
		out << '[';
		int i;
		for (i = 0; i < 7; i++) {
			if ((p.coefs >> 7-i) & 1) {
				out << "x^" << 7-i;
				break;
			}
		}
		if (i == 7) {
			out << (p.coefs & 1) << ']';
			return out;
		}
		for (i += 1; i < 7; i++)
			if ((p.coefs >> 7-i) & 1)
				out << " + x^" << 7-i;
		if (p.coefs & 1)
			out << " + 1";
		out << ']';
		return out;
	}
	#endif

// same as normal mult?
	// polynomial multiplication
	Poly Poly::operator*(Poly other) {
		// product with overflow
		uint16_t prod = 0;
		for (int i = 0; i < 8; i++)
			prod ^= (coefs * ((other.coefs >> i) & 1)) << i;
		// modulous by long division
		int msig, mult;
		while (prod >> 8) {
			// index of most significant bit
			msig = 15;
			while (!((prod >> msig) & 1))
				msig--;
			// multiply divisor by multiple, then subtract from product
			mult = msig - 8;
			prod ^= MOD << mult;
		}
		return Poly((uint8_t) prod);
	}

};

#ifdef TEST
#include <iostream>
#include <cassert>
int main() {
	aes::Poly a = 0x57, b = 0x83;
	std::cout << "a: " << a << std::endl;
	std::cout << "b: " << b << std::endl;
	std::cout << "a*b: " << a*b << std::endl;
	std::cout << "a*b (raw): " << aes::Poly((uint8_t)((0x57*0x83) % 0x11B)) << std::endl;
	assert(a + b == 0xD4);
	assert(a * b == 0xC1);
	a.circshl(2);
	uint8_t test = 12;
	aes::Poly testPoly = test;
}
#endif
