#include <cstdint>
#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif

namespace aes {

	class Poly {
		uint8_t coefs;

		public:

		static constexpr uint16_t MOD = 0x11B;
		constexpr Poly(uint8_t coefs = 0): coefs(coefs) {}

		constexpr operator uint8_t() {
			return coefs;
		}

		uint8_t raw() const {
			return coefs;
		}

		// polynomial addition
		constexpr Poly operator+(Poly other) {
			return coefs ^ other.coefs;
		}

		// same as subtraction
		constexpr Poly operator-(Poly other) {
			return operator+(other);
		}

		// circular left shift
		constexpr Poly circshl(int shamt) {
			return (coefs << shamt) | (coefs >> (8 - shamt));
		}

		constexpr Poly operator*(Poly other);

		constexpr Poly inv();

		#ifndef NDEBUG
		friend std::ostream &operator<<(std::ostream &, Poly);
		#endif

	};

	// polynomial multiplication
	constexpr Poly Poly::operator*(Poly other) {
		// product with overflow
		uint16_t prod = 0;
		for (int i = 0; i < 8; i++)
			prod ^= (coefs * ((other.coefs >> i) & 1)) << i;
		// modulous by long division
		int msig = 0, mult = 0; //initialized to 0 for constexpr
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

	constexpr Poly Poly::inv() {
		if (coefs == 0)
			return 0;
		uint8_t i = 1;
		for (; operator*(i) != 1; i++) {
			assert(i != 0); // No inverse found
		}
		return i;
	}

	#ifndef NDEBUG
	std::ostream &operator<<(std::ostream &out, Poly p) {
		out << '[';
		int i;
		for (i = 0; i < 7; i++) {
			if ((p.coefs >> (7-i)) & 1) {
				out << "x^" << 7-i;
				break;
			}
		}
		if (i == 7) {
			out << (p.coefs & 1) << ']';
			return out;
		}
		for (i += 1; i < 7; i++)
			if ((p.coefs >> (7-i)) & 1)
				out << " + x^" << 7-i;
		if (p.coefs & 1)
			out << " + 1";
		out << ']';
		return out;
	}
	#endif

};

#ifdef TEST_POLY
#include <iostream>
int main() {
	aes::Poly a = 0x57, b = 0x83;
	assert(a + b == 0xD4);
	assert(a * b == 0xC1);
	assert(a * a.inv() == 1);
	assert(b * b.inv() == 1);
	assert(a * b * a.inv() == b);
	a.circshl(2);
	std::cout << "poly test complete" << std::endl;
}
#endif
