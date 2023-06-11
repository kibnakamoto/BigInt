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
// File not done.
// the functions were tested and all working and fast. This was for further testing and there is none yet.

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


		/////////// TEST BOOLEAN OPERATORS
		virtual bool test_bool_not_eq() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num1 = "2043835430954379549087549803752094857685945098653096065479"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num != num1;
		}

		virtual bool test_bool_not() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return !num;
		}

		virtual bool test_bool_not0() {
			BigUint<bitsize> num = "0"; // 1-bit hex
			std::cout << "\ncorrect answer is false";
			return !num;
		}

		virtual bool test_bool_less() {
			BigUint<bitsize> num =  "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			BigUint<bitsize> num1 = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num < num1;
		}

		virtual bool test_bool_less_eq() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			BigUint<bitsize> num1 = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num <= num1;
		}

		virtual bool test_bool_and() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			BigUint<bitsize> num2 = "1";
			std::cout << "\ncorrect answer is true";
			return num && num2; // true, because both are 1 or bigger
		}

		virtual bool test_bool_or() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			BigUint<bitsize> num2 = "0";
			std::cout << "\ncorrect answer is true";
			return num || num2; // true, because num is 1 or bigger
		}

		virtual bool test_bool_equal_eq() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			BigUint<bitsize> num2 = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num == num2; // true, because they are equal
			
		}

		virtual bool test_bool_greater() {
			BigUint<bitsize> num =  "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num1 = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num > num1; // true, because num is bigger
		}

		virtual bool test_bool_greater_eq() {
			BigUint<bitsize> num =  "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num1 = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num >= num1; // true, because num is bigger
		}

		/////////// TEST BITWISE OPERATORS (15 functions)
		virtual bool test_bitwise_not() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = ~num; // 232-bit hex
			std::cout << std::hex << std::endl << "			   output: " << num2;
			std::cout << std::endl << "the correct answer: ffffffdcc89e97ccaadfb99fcba7ccb8bf7b6ea6be89acbeecfd876ce6dba99e";
			return optostr(num2.__get_op()) == "ffffffdcc89e97ccaadfb99fcba7ccb8bf7b6ea6be89acbeecfd876ce6dba99e";
		}
		virtual bool test_bitwise_and() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = "13439085734205734"; // 232-bit hex
			BigUint<bitsize> num3 = num & num2; // 232-bit hex
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 11000081310205620";
			return optostr(num3.__get_op()) == "11000081310205620";
			
		}
		virtual bool test_bitwise_and_eq() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
									
			BigUint<bitsize> num2 = "13439085734205734"; // 232-bit hex
			num &= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 11000081310205620";
			return optostr(num.__get_op()) == "11000081310205620";
		}
		virtual bool test_bitwise_xor() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = "4423905842305789076053609538769048324785042058710953420857"; // 232-bit hex
			BigUint<bitsize> num3 = num ^ num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 6714f130716577cf67540b53d278f201117331d645335a099a4a665e36";
			return optostr(num3.__get_op()) == "6714f130716577cf67540b53d278f201117331d645335a099a4a665e36";
		}
		virtual bool test_bitwise_xor_eq() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = "4423905842305789076053609538769048324785042058710953420857"; // 232-bit hex
			num ^= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 6714f130716577cf67540b53d278f201117331d645335a099a4a665e36";
			return optostr(num.__get_op()) == "6714f130716577cf67540b53d278f201117331d645335a099a4a665e36";
		}
		virtual bool test_bitwise_rshift() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = num >> BigUint<bitsize>(2);
			std::cout << std::hex << std::endl << "			   output: " << num2;
			std::cout << std::endl << "the correct answer: 8cdd85a0cd54811980d160cd1d0212456505d94d044c09e24c6491598";
			return optostr(num2.__get_op()) == "8cdd85a0cd54811980d160cd1d0212456505d94d044c09e24c6491598";
			
		}
		virtual bool test_bitwise_rshift_eq() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			num >>= BigUint<bitsize>(2);
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 8cdd85a0cd54811980d160cd1d0212456505d94d044c09e24c6491598";
			return optostr(num.__get_op()) == "8cdd85a0cd54811980d160cd1d0212456505d94d044c09e24c6491598";
			
		}
		virtual bool test_bitwise_lshift() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = num << BigUint<bitsize>(20);
			std::cout << std::hex << std::endl << "			   output: " << num2;
			std::cout << std::endl << "the correct answer: 233761683355204660345833474084915941765341130278931924566100000";
			return optostr(num2.__get_op()) == "233761683355204660345833474084915941765341130278931924566100000";
		}	
		virtual bool test_bitwise_lshift_eq() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			num <<= 20;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 233761683355204660345833474084915941765341130278931924566100000";
			return optostr(num.__get_op()) == "233761683355204660345833474084915941765341130278931924566100000";
		}
		virtual bool test_bitwise_or() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = "4423905842305789076053609538769048324785042058710953420857"; // 232-bit hex
			BigUint<bitsize> num3 = num | num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 6737f178737577cf67745b73d778f691597377d745335a799b5b665e77";
			return optostr(num3.__get_op()) == "6737f178737577cf67745b73d778f691597377d745335a799b5b665e77";
		}
		virtual bool test_bitwise_or_eq() {
			BigUint<bitsize> num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = "4423905842305789076053609538769048324785042058710953420857"; // 232-bit hex
			num |= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 6737f178737577cf67745b73d778f691597377d745335a799b5b665e77";
			return optostr(num.__get_op()) == "6737f178737577cf67745b73d778f691597377d745335a799b5b665e77";
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

		virtual bool test_addition_eq() {
			BigUint<bitsize> num="2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			BigUint<bitsize> num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			num += num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
			return optostr(num.__get_op()) == "466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
		}

		virtual bool test_subtraction() {
			// python3	SUBTRACTION TEST, CORRECT for all cases:
			/// hex(0x134395843534503845740957390690572-0x2337616833552046603458334740849159417653411302789319245661 + (1 << 256))
			///'0xffffffdcc89e97ccaadfb99fcba7ccb9f3b4c6ea11ceb043443e1ce07744af11'
			BigUint<bitsize> num="134395843534503845740957390690572"; // 132-bit hex
			BigUint<bitsize> num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num3 = num - num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 233761683355204660345833460c4b3915ee314fbcbbc1e31f88bb50ef";
			return optostr(num3.__get_op()) == "233761683355204660345833460c4b3915ee314fbcbbc1e31f88bb50ef";
		}

		virtual bool test_subtraction_eq() {
			BigUint<bitsize> num="134395843534503845740957390690572"; // 132-bit hex
			BigUint<bitsize> num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			num2 -= num;
			std::cout << std::hex << std::endl << "			   output: " << num2;
			std::cout << std::endl << "the correct answer: 233761683355204660345833460c4b3915ee314fbcbbc1e31f88bb50ef";
			return optostr(num2.__get_op()) == "233761683355204660345833460c4b3915ee314fbcbbc1e31f88bb50ef";
		}

		virtual bool test_mul() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num2="2000";
			BigUint<bitsize> num3 = num * num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 11320a8fe11090c25cf98f1109ca80baf894f6c6a6c64834dfd346be2d5d0";
			return optostr(num3.__get_op()) == "11320a8fe11090c25cf98f1109ca80baf894f6c6a6c64834dfd346be2d5d0";
		}

		virtual bool test_mul_eq() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num2="2000"; // 132-bit hex
			num *= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 11320a8fe11090c25cf98f1109ca80baf894f6c6a6c64834dfd346be2d5d0";
			return optostr(num.__get_op()) == "11320a8fe11090c25cf98f1109ca80baf894f6c6a6c64834dfd346be2d5d0";
		}

		virtual bool test_div() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num2="2000309429034920423834902";
			BigUint<bitsize> num3 = num / num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 119b95f9328dfcca5df0870060df27aaf0";
			return optostr(num3.__get_op()) == "119b95f9328dfcca5df0870060df27aaf0";
		}

		virtual bool test_div_eq() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num2="2000309429034920423834902";
			num /= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 119b95f9328dfcca5df0870060df27aaf0";
			return optostr(num.__get_op()) == "119b95f9328dfcca5df0870060df27aaf0";
		}

		virtual bool test_mod() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num2="23049";
			BigUint<bitsize> num3 = num % num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 90926";
			return optostr(num3.__get_op()) == "90926";
		}

		virtual bool test_mod_eq() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num2="23049";
			num /= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 90926";
			return optostr(num.__get_op()) == "90926";
		}

		virtual bool test_pow() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			BigUint<bitsize> num2="2";
			BigUint<bitsize> num3 = pow(num, num2);
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: b9fe143015ff9a71ef6f1d05068d2b9a565f8dbb28b4c8bdb2b2416d5a6d50c1 ";
			return optostr(num3.__get_op()) == "b9fe143015ff9a71ef6f1d05068d2b9a565f8dbb28b4c8bdb2b2416d5a6d50c1";
		}

		/////////// TEST INCREMENT AND DECREMENT
		virtual bool test_inc() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			num++;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 2337616833552046603458334740849159417653411302789319245662";
			return optostr(num.__get_op()) == "2337616833552046603458334740849159417653411302789319245662";
		}

		virtual bool test_dec() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			num--;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 2337616833552046603458334740849159417653411302789319245660";
			return optostr(num.__get_op()) == "2337616833552046603458334740849159417653411302789319245660";
		}

		/////////// TEST INDEX

		virtual bool test_index_bit() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			bool first_index = num[(uint32_t)0];
			std::cout << std::endl << "the correct answer (index 0): 1";
			return first_index == 1;
		}

		virtual bool test_index_uint64() {
			BigUint<bitsize> num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			uint64_t first_index = num[(uint16_t)0];
			return first_index == 0x2337616833ULL;
		}
};

// Benchmark test speed of Bigints
template<uint16_t bitsize>
class Benchmark : public Test<bitsize>
{
	public:
	// assignment operators (11 functions)
	// test_chararr_assignment, test_str_assignment, test_long_chararr_assignment, test_long_str_assignment, test_long_hex_assignment, test_long_nonint_assignment, test_long_nonhex_assignment, test_nonint_assignment, test_nonhex_assignment, test_int_assignment, test_long_int_assignment,  consteval_test_long_int_assignment

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

	// boolean operators (9 functions)
	// test_bool_and, test_bool_or, test_bool_equal_eq, test_bool_not, test_bool_not_eq, test_bool_less, test_bool_less_eq, test_bool_greater, test_bool_greater_eq

	// bitwise operators (15 functions)
	// test_bitwise_not, bitwise_and, bitwise_and_eq, bitwise_xor, bitwise_xor_eq, bitwise_rshift_uint16,
	// bitwise_rshift_eq_uint16, bitwise_lshift_uint16, bitwise_lshift_eq_uint16, bitwise_or, bitwise_or_eq

	// basic arithmetic (11 functions)
	// test_addition, test_addition_eq, test_subtraction, test_subtraction_eq, test_mul, test_mul_eq, test_div, test_div_eq, test_mod, test_mod_eq, test_pow
	bool test_addition() { return 0; }

	// incrementation and decrementation (2 functions)
	// test_inc, test_dec
	
	// index operators (/2 functions)
	// test_index_bit, test_index_uint64
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
