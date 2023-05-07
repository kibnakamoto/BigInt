// Test the code

#include <iostream>
#include <string>
#include <stdint.h>
#include<random>

#include "bigint.h"

// TODO: set testing with different bitsizes, e.g. one is 256 while other is 512. test operators using this
// TODO: add looping and assign everything non-constexpr to rand_str

template<uint16_t bitsize>
class Test : public BigInt<bitsize>
{
	protected:
		static constexpr const char hex_digits[17] = "0123456789abcdef";
		std::string optostr(uint64_t *__op)
		{
			// convert operator array to string
			std::string str = "";
			for(uint16_t i=0;i<BigInt<bitsize>::op_size;i++) str += std::to_string(__op[i]);
			return str;
		}

		std::string rand_str(uint16_t _bitsize) {
		    // random byte using Mersenne Twister, generate random bitset length, then generate a random string of that random length
    		std::random_device randDev;
    		std::mt19937 generator(randDev() ^ time(NULL));
    		std::uniform_int_distribution<uint16_t> distr(0, _bitsize);
    		std::uniform_int_distribution<bool> distr2;
			uint16_t length = distr(generator);
			bool bitset[length];
			uint16_t hexlen = length%4==0 ? length/4 : length/4+1;
			uint8_t hexset[hexlen];
			std::string str;
			char* ptr = &str[0];
			for(uint16_t i=0;i<length;i++) {
				if(i%4) hexset[i] = 0x0000;
				bitset[i] = distr2(generator);
				hexset[i/4] <<= 1;
				hexset[i/4] |= bitset[i];
				if(i%4==3) {
					ptr++ = hex_digits[hexset[i/4] >> 4];
					ptr++ = hex_digits[hexset[i/4] & 0x0f];
				}
			}
			
			return str;
		}

	public:
		std::string _op;
		Test() {
			// convert operator array to string
			_op = optostr(BigInt<bitsize>::op);
		}

		/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT, HEX)
		void test_chararr_assignment() {
			BigInt<bitsize> num="256";
		}
		
		void test_str_assignment() {
			BigInt<bitsize> num = std::string("256");
		}
		
		void test_long_chararr_assignment() {
			BigInt<bitsize> num = "56539106072908298546665520023773392506479484700019806659891398441363832832";
		}
		
		void test_long_str_assignment() {
			BigInt<bitsize> num = std::string("56539106072908298546665520023773392506479484700019806659891398441363832832");
		}
		
		void test_long_hex_assignment() {
			BigInt<bitsize> num = std::string("0x5653910607290829E8546665D5A200237733925064794847000198066598913984FF41363832B832F");
		}
		
		// false input
		void test_long_nonint_assignment() {
			BigInt<bitsize> num = std::string("5653910#jrkyls607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398");
			BigInt<bitsize> num2 = "5653910#jrkyls607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398";
		}
		
		// false input
		void test_long_nonhex_assignment() {
			BigInt<bitsize> num = std::string("0x5653910#607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398");
			BigInt<bitsize> num2 = "0x5653910#607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398";
		}
		
		// false input
		void test_nonint_assignment() {
			BigInt<bitsize> num = std::string("t3f");
			BigInt<bitsize> num2 = "t3f";
		}
		
		// false input
		void test_nonhex_assignment() {
			BigInt<bitsize> num = std::string("0xaTf");
			BigInt<bitsize> num2 = "0xaTf";
		}
		
		/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT, HEX)
		void test_int_assignment() {
			BigInt<bitsize> num = BigInt<bitsize>(256);
		}
		
		void test_long_int_assignment() {
    		static constexpr const __uint128_t num_long_int = ((__uint128_t)0xabcde0123456789fU<<64)|0xffffffffffffffffU;
    		static constexpr const __uint128_t num_long_int2 = ((__uint128_t)0xffffffffffffffffU<<64)|0xffffffffffffffffU;
			BigInt<bitsize> num = BigInt<bitsize>(num_long_int, num_long_int2);
		}

		/////////// TEST BOOLEAN OPERATORS
		bool test_bool_not_equal() {
			BigInt<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigInt<bitsize> num1 = "2043835430954379549087549803752094857685945098653096065479"; // 232-bit hex
			return num != num1;
		}

		bool test_bool_not_equal_equal() {
			BigInt<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigInt<bitsize> num1 = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			return num != num1;
		}

		bool test_bool_not() {
			BigInt<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			return !num;
		}

		bool test_bool_not0() {
			BigInt<bitsize> num = "0"; // 232-bit hex
			return !num;
		}
		
		/////////// TEST BASIC ARITHMETIC
		bool test_addition() {
			BigInt<bitsize> num="2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigInt<bitsize> num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigInt<bitsize> num3 = num + num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
			return optostr(num3.op) == "466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
		}
};

int main()
{
	// TODO: call the functions and add print functions to all of them
	return 0;
}
