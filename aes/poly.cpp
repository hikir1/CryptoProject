#include <cstdint>
#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif

#include "poly.hpp"

namespace aes {

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
