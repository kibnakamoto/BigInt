// Test the code

#include <iostream>
#include <string>
#include <stdint.h>

#include "bigint.h"

/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT, HEX)
void test_chararr_assignment() {
	uint256_t num="256";
}

void test_str_assignment() {
	uint256_t num = std::string("256");
}

void test_long_chararr_assignment() {
	uint256_t num = "56539106072908298546665520023773392506479484700019806659891398441363832832";
}

void test_long_str_assignment() {
	uint256_t num = std::string("56539106072908298546665520023773392506479484700019806659891398441363832832");
}

void test_long_hex_assignment() {
	uint256_t num = std::string("0x5653910607290829E8546665D5A200237733925064794847000198066598913984FF41363832B832F");
}

// false input
void test_long_nonint_assignment() {
	uint256_t num = std::string("5653910#jrkyls607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398");
	uint256_t num = "5653910#jrkyls607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398";
}

// false input
void test_long_nonhex_assignment() {
	uint256_t num = std::string("0x5653910#607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398");
	uint256_t num = "0x5653910#607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398";
}

// false input
void test_nonint_assignment() {
	uint256_t num = std::string("t3f");
	uint256_t num = "t3f";
}

// false input
void test_nonhex_assignment() {
	uint256_t num = std::string("0xaTf");
	uint256_t num = "0xaTf";
}

/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT, HEX)
void test_int_assignment() {
	uint256_t num = uint256_t<1>(256);
}

void test_long_int_assignment() {
	uint256_t num = uint256_t<1>(0xabcde0123456789fffffffffffffffff,0xffffffffffffffffffffffffffff);
}

int main()
{
	// TODO: call the functions
	return 0;
}
