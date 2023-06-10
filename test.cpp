// Test the code
#ifndef TEST_CPP
#define TEST_CPP

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdint.h>
#include <random>
#include <chrono>

#include "bigint.h"

// test everything for multiple Bitsizes (different op_size)

using namespace BigInt;

template<uint16_t bitsize>
class Benchmark;

// Test Bigint for values
template<uint16_t bitsize>
class Test : public BigUint<bitsize>
{
	friend class Benchmark<bitsize>;
	protected:
		static constexpr const uint16_t op_size = BigUint<bitsize>::__get_op_size();
		uint64_t* op = BigUint<bitsize>::__get_op();
		static constexpr const char hex_digits[17] = "0123456789abcdef";
		std::string optostr(uint64_t *__op)
		{
			// convert operator array to string
			std::string str = "";
			for(uint16_t i=0;i<op_size;i++) str += std::to_string(__op[i]);
			return str;
		}

		std::string rand_str(uint16_t _bitsize)
		{
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
		explicit Test() {
			// convert operator array to string
			_op = optostr(BigUint<bitsize>::op);
		}

		/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT, HEX)
		virtual void test_chararr_assignment() {
			BigUint<bitsize> num="256";
		}
		
		virtual void test_str_assignment() {
			BigUint<bitsize> num = std::string("256");
		}
		
		virtual void test_long_chararr_assignment() {
			BigUint<bitsize> num = "56539106072908298546665520023773392506479484700019806659891398441363832832";
		}
		
		virtual void test_long_str_assignment() {
			BigUint<bitsize> num = std::string("56539106072908298546665520023773392506479484700019806659891398441363832832");
		}
		
		virtual void test_long_hex_assignment() {
			BigUint<bitsize> num = std::string("0x5653910607290829E8546665D5A200237733925064794847000198066598913984FF41363832B832F");
		}
		
		// false input
		virtual void test_long_nonint_assignment() {
			BigUint<bitsize> num = std::string("5653910#jrkyls607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398");
			BigUint<bitsize> num2 = "5653910#jrkyls607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398";
		}
		
		// false input
		virtual void test_long_nonhex_assignment() {
			BigUint<bitsize> num = std::string("0x5653910#607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398");
			BigUint<bitsize> num2 = "0x5653910#607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398";
		}
		
		// false input
		virtual void test_nonint_assignment() {
			BigUint<bitsize> num = std::string("t3f");
			BigUint<bitsize> num2 = "t3f";
		}
		
		// false input
		virtual void test_nonhex_assignment() {
			BigUint<bitsize> num = std::string("0xaTf");
			BigUint<bitsize> num2 = "0xaTf";
		}

		/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT (amount of variadic parameters), INT)
		virtual void test_int_assignment() {
			BigUint<bitsize> num = BigUint<bitsize>(1, 256);
		}
		
		// test variadic constructor of type __uint128_t
		virtual void test_long_int_assignment() {
    		static constexpr const __uint128_t num_long_int = ((__uint128_t)0xabcde0123456789fU<<64)|0xffffffffffffffffU;
    		static constexpr const __uint128_t num_long_int2 = ((__uint128_t)0xffffffffffffffffU<<64)|0xffffffffffffffffU;
			BigUint<bitsize> num = BigUint<bitsize>(2, num_long_int, num_long_int2);
		}

		// test consteval of the above function (bigint::assign_conste)
		virtual void consteval_test_long_int_assignment() {
			constexpr BigUint<bitsize> num = BigUint<bitsize>().template
										     assign_conste<__uint128_t>(((__uint128_t)0xabcde0123456789fu<<64)|0xffffffffffffffffu,
																		((__uint128_t)0xffffffffffffffffu<<64)|0xffffffffffffffffu);
		}

		// TODO: make uint64_t * constructor + assign_op

		/////////// TEST BOOLEAN OPERATORS
		virtual bool test_bool_not_equal() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num1 = "2043835430954379549087549803752094857685945098653096065479"; // 232-bit hex
			return num != num1;
		}

		virtual bool test_bool_not_equal_equal() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num1 = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			return num != num1;
		}

		virtual bool test_bool_not() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			return !num;
		}

		virtual bool test_bool_not0() {
			BigUint<bitsize> num = "0"; // 1-bit hex
			return !num;
		}

		virtual bool test_bool_less() {
			BigUint<bitsize> num =  "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			BigUint<bitsize> num1 = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			return num < num1;
		}

		virtual bool *test_bool_less_equal() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			BigUint<bitsize> num1 = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			bool *ret;
			*ret = (num <= num1);
			if(ret) {
				BigUint<bitsize> num2 = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
				*ret++ = num1 <= num2; // should be equal
			}
			return ret;
		}
		
		/////////// TEST BASIC ARITHMETIC
		virtual bool test_addition() {
			BigUint<bitsize> num="2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num3 = num + num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
			return optostr(num3.__get_op()) == "466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
		}

		virtual bool test_subtraction() {
			// python3	SUBTRACTION TEST, CORRECT for all cases:
			/// hex(0x134395843534503845740957390690572-0x2337616833552046603458334740849159417653411302789319245661 + (1 << 256))
			///'0xffffffdcc89e97ccaadfb99fcba7ccb9f3b4c6ea11ceb043443e1ce07744af11'
			BigUint<bitsize> num="134395843534503845740957390690572"; // 132-bit hex
			BigUint<bitsize> num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num3 = num - num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
			return optostr(num3.__get_op()) == "466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
		}
};

// Benchmark test speed of Bigints
template<uint16_t bitsize>
class Benchmark : public Test<bitsize>
{
	public:
	// assignment operators (11 functions)
	// test_chararr_assignment, test_str_assignment, test_long_chararr_assignment, test_long_str_assignment, test_long_hex_assignment, test_long_nonint_assignment, test_long_nonhex_assignment, test_nonint_assignment, test_nonhex_assignment, test_int_assignment, test_long_int_assignment,  consteval_test_long_int_assignment

	// boolean operators (4 functions)
	// test_bool_not_equal, test_bool_not_equal_equal, test_bool_not, test_bool_not0

	// bitwise operators (15 functions)
	// test_bitwise_not, bitwise_and, bitwise_and_eq, bitwise_xor, bitwise_xor_eq, bitwise_rshift_uint16,
	// bitwise_rshift_eq_uint16, bitwise_lshift_uint16, bitwise_lshift_eq_uint16, bitwise_or, bitwise_or_eq

	// basic arithmetic (3 functions)
	// test_addition, test_addition_eq, test_subtraction, test_subtraction_eq, test_mul, test_mul_eq, test_div
	bool test_addition() { return 0; }

	// incrementation and decrementation (2 functions)
	// increment, decrement
	
	// index operators (/2 functions)
	// index_bit, index_uint64
};

int main()
{
	constexpr const static uint16_t bitsize = 256;
	static uint32_t count=100;
	Test<bitsize> tester = Test<bitsize>();

	/************* TODO: VALUE TESTS *************/

	/************* BENCHMARK TESTS *************/
	std::chrono::system_clock::time_point timer_starts[30];
	std::chrono::system_clock::time_point timer_ends[30];

	Benchmark<bitsize> benchmarker = Benchmark<bitsize>();

	// test char-array hexstring assignment
	timer_starts[0] = std::chrono::high_resolution_clock::now();
	//for(uint32_t i=0;i<count;i++) benchmarker.test_chararr_assignment();
	timer_ends[0] = std::chrono::high_resolution_clock::now();

	// test non-integer hexstring assignment
	timer_starts[1] = std::chrono::high_resolution_clock::now();
	//for(uint32_t i=0;i<count;i++) benchmarker.test_str_assignment();
	timer_ends[1] = std::chrono::high_resolution_clock::now();
	
	return 0;
}

#endif /* TEST_CPP*/
