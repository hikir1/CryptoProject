#include <cstdint>
#ifndef NDEBUG
#include <iostream>
#endif

// TODO: 


namespace aes {

	class Poly {
		const uint8_t coefs;

		public:

		constexpr Poly(uint8_t coefs): coefs(coefs) {}

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

		constexpr Poly operator*(Poly other);

		Poly inv();

		#ifndef NDEBUG
		friend std::ostream &operator<<(std::ostream &, Poly);
		#endif

	};

// same as normal mult?
	// polynomial multiplication
	constexpr Poly Poly::operator*(Poly other) {
		// product with overflow
		uint16_t prod = 0;
		for (int i = 0; i < 8; i++)
			prod ^= (coefs * ((other.coefs >> i) & 1)) << i;
		// modulous by long division
		constexpr uint16_t MOD = 0x11B;
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

	// calculate inverses at compile time (if not debugging, bc very slow)
	namespace {
		struct InvArray {
			uint8_t vals[1 << 8];
			#ifdef NDEBUG
			constexpr
			#endif
			InvArray() : vals() {
				vals[0] = 0;
				uint8_t mult = 0; // initialized to 0 for constexpr
				for (uint8_t i = 1; i != 0; i++) {
					for (mult = 1; Poly(mult) * Poly(i) != 1; mult++);
					vals[i] = mult;
				}
			}
		}
		#ifdef NDEBUG
		constexpr
		#else
		const
		#endif
		INVARR;
	}

	inline Poly Poly::inv() {
		return INVARR.vals[coefs];
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

};

#ifdef TEST
#include <iostream>
#include <cassert>
int main() {
	aes::Poly a = 0x57, b = 0x83;
	assert(a + b == 0xD4);
	assert(a * b == 0xC1);
	assert(a * a.inv() == 1);
	assert(b * b.inv() == 1);
	assert(a * b * a.inv() == b);
	a.circshl(2);
}
#endif
