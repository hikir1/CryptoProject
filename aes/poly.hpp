#ifndef _POLY_HPP_
#define _POLY_HPP_

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


};

#endif
