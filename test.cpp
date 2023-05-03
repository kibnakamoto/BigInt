// Test the code

#include <iostream>
#include <string>
#include <stdint.h>

#include "bigint.h"

template<uint16_t bitsize>
class Test : public BigInt<bitsize>
{
	protected:
		std::string optostr(uint64_t *__op)
		{
			// convert operator array to string
			std::string str = "";
			for(uint16_t i=0;i<op_size;i++) str += std::to_string(__op[i]);
			return str;
		}

	public:
		std::string _op;
		Test() {
			// convert operator array to string
			_op = optostr(op);
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
			BigInt<bitsize> num = BigInt<bitsize><1>(256);
		}
		
		void test_long_int_assignment() {
			BigInt<bitsize> num = BigInt<bitsize><1>(0xabcde0123456789fffffffffffffffff,0xffffffffffffffffffffffffffff);
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
