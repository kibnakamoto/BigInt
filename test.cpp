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
// the functions were tested and all working and fast.

using namespace BigInt;

// Test Bigint for values
template<typename _type, typename bitsize_t, bitsize_t bitsize>
class Test : public _type::BigUint
{
	protected:
		static constexpr const bitsize_t op_size = _type::__get_op_size();
		uint64_t* op = _type::__get_op();
		std::string optostr(uint64_t *__op)
		{
			// convert operator array to string
			std::string str = "";
			for(bitsize_t i=0;i<op_size;i++) str += std::to_string(__op[i]);
			return str;
		}

	public:
		std::string _op;
		explicit Test() {
			// convert operator array to string
			_op = optostr(_type::op);
		}

		/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT, HEX)
		virtual void test_chararr_assignment() {
			_type num="256";
		}
		
		virtual void test_str_assignment() {
			_type num = std::string("256");
		}
		
		virtual void test_long_chararr_assignment() {
			_type num = "56539106072908298546665520023773392506479484700019806659891398441363832832";
		}
		
		virtual void test_long_str_assignment() {
			_type num = std::string("56539106072908298546665520023773392506479484700019806659891398441363832832");
		}
		
		virtual void test_long_hex_assignment() {
			_type num = std::string("0x5653910607290829E8546665D5A200237733925064794847000198066598913984FF41363832B832F");
		}
		
		// false input
		virtual void test_long_nonint_assignment() {
			_type num = std::string("5653910#jrkyls607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398");
			_type num2 = "5653910#jrkyls607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398";
		}
		
		// false input
		virtual void test_long_nonhex_assignment() {
			_type num = std::string("0x5653910#607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398");
			_type num2 = "0x5653910#607290829E8546665D5A2002377339250647948giutphgq384700019806659f891398";
		}
		
		// false input
		virtual void test_nonint_assignment() {
			_type num = std::string("t3f");
			_type num2 = "t3f";
		}
		
		// false input
		virtual void test_nonhex_assignment() {
			_type num = std::string("0xaTf");
			_type num2 = "0xaTf";
		}

		/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT (amount of variadic parameters), INT)
		virtual void test_int_assignment() {
			_type num = _type(1, 256);
		}
		
		// test variadic constructor of type __uint128_t
		virtual void test_long_int_assignment() {
    		static constexpr const __uint128_t num_long_int = ((__uint128_t)0xabcde0123456789fU<<64)|0xffffffffffffffffU;
    		static constexpr const __uint128_t num_long_int2 = ((__uint128_t)0xffffffffffffffffU<<64)|0xffffffffffffffffU;
			_type num = _type(2, num_long_int, num_long_int2);
		}

		// test consteval of the above function (bigint::assign_conste)
		virtual void consteval_test_long_int_assignment() {
			constexpr _type num = _type().template
										     assign_conste<__uint128_t>(((__uint128_t)0xabcde0123456789fu<<64)|0xffffffffffffffffu,
																		((__uint128_t)0xffffffffffffffffu<<64)|0xffffffffffffffffu);
		}


		/////////// TEST BOOLEAN OPERATORS
		virtual bool test_bool_not_eq() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num1 = "2043835430954379549087549803752094857685945098653096065479"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num != num1;
		}

		virtual bool test_bool_not() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return !num;
		}

		virtual bool test_bool_not0() {
			_type num = "0"; // 1-bit hex
			std::cout << "\ncorrect answer is false";
			return !num;
		}

		virtual bool test_bool_less() {
			_type num =  "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			_type num1 = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num < num1;
		}

		virtual bool test_bool_less_eq() {
			_type num = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			_type num1 = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num <= num1;
		}

		virtual bool test_bool_and() {
			_type num = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			_type num2 = "1";
			std::cout << "\ncorrect answer is true";
			return num && num2; // true, because both are 1 or bigger
		}

		virtual bool test_bool_or() {
			_type num = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			_type num2 = "0";
			std::cout << "\ncorrect answer is true";
			return num || num2; // true, because num is 1 or bigger
		}

		virtual bool test_bool_equal_eq() {
			_type num = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			_type num2 = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num == num2; // true, because they are equal
			
		}

		virtual bool test_bool_greater() {
			_type num =  "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num1 = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num > num1; // true, because num is bigger
		}

		virtual bool test_bool_greater_eq() {
			_type num =  "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num1 = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
			std::cout << "\ncorrect answer is true";
			return num >= num1; // true, because num is bigger
		}

		/////////// TEST BITWISE OPERATORS (15 functions)
		virtual bool test_bitwise_not() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = ~num; // 232-bit hex
			std::cout << std::hex << std::endl << "			   output: " << num2;
			std::cout << std::endl << "the correct answer: ffffffdcc89e97ccaadfb99fcba7ccb8bf7b6ea6be89acbeecfd876ce6dba99e";
			return optostr(num2.__get_op()) == "ffffffdcc89e97ccaadfb99fcba7ccb8bf7b6ea6be89acbeecfd876ce6dba99e";
		}
		virtual bool test_bitwise_and() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = "13439085734205734"; // 232-bit hex
			_type num3 = num & num2; // 232-bit hex
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 11000081310205620";
			return optostr(num3.__get_op()) == "11000081310205620";
			
		}
		virtual bool test_bitwise_and_eq() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
									
			_type num2 = "13439085734205734"; // 232-bit hex
			num &= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 11000081310205620";
			return optostr(num.__get_op()) == "11000081310205620";
		}
		virtual bool test_bitwise_xor() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = "4423905842305789076053609538769048324785042058710953420857"; // 232-bit hex
			_type num3 = num ^ num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 6714f130716577cf67540b53d278f201117331d645335a099a4a665e36";
			return optostr(num3.__get_op()) == "6714f130716577cf67540b53d278f201117331d645335a099a4a665e36";
		}
		virtual bool test_bitwise_xor_eq() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = "4423905842305789076053609538769048324785042058710953420857"; // 232-bit hex
			num ^= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 6714f130716577cf67540b53d278f201117331d645335a099a4a665e36";
			return optostr(num.__get_op()) == "6714f130716577cf67540b53d278f201117331d645335a099a4a665e36";
		}
		virtual bool test_bitwise_rshift() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = num >> _type(2);
			std::cout << std::hex << std::endl << "			   output: " << num2;
			std::cout << std::endl << "the correct answer: 8cdd85a0cd54811980d160cd1d0212456505d94d044c09e24c6491598";
			return optostr(num2.__get_op()) == "8cdd85a0cd54811980d160cd1d0212456505d94d044c09e24c6491598";
			
		}
		virtual bool test_bitwise_rshift_eq() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			num >>= _type(2);
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 8cdd85a0cd54811980d160cd1d0212456505d94d044c09e24c6491598";
			return optostr(num.__get_op()) == "8cdd85a0cd54811980d160cd1d0212456505d94d044c09e24c6491598";
			
		}
		virtual bool test_bitwise_lshift() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = num << _type(20);
			std::cout << std::hex << std::endl << "			   output: " << num2;
			std::cout << std::endl << "the correct answer: 233761683355204660345833474084915941765341130278931924566100000";
			return optostr(num2.__get_op()) == "233761683355204660345833474084915941765341130278931924566100000";
		}	
		virtual bool test_bitwise_lshift_eq() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			num <<= 20;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 233761683355204660345833474084915941765341130278931924566100000";
			return optostr(num.__get_op()) == "233761683355204660345833474084915941765341130278931924566100000";
		}
		virtual bool test_bitwise_or() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = "4423905842305789076053609538769048324785042058710953420857"; // 232-bit hex
			_type num3 = num | num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 6737f178737577cf67745b73d778f691597377d745335a799b5b665e77";
			return optostr(num3.__get_op()) == "6737f178737577cf67745b73d778f691597377d745335a799b5b665e77";
		}
		virtual bool test_bitwise_or_eq() {
			_type num = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = "4423905842305789076053609538769048324785042058710953420857"; // 232-bit hex
			num |= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 6737f178737577cf67745b73d778f691597377d745335a799b5b665e77";
			return optostr(num.__get_op()) == "6737f178737577cf67745b73d778f691597377d745335a799b5b665e77";
		}	
		
		/////////// TEST BASIC ARITHMETIC
		virtual bool test_addition() {
			_type num="2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			_type num3 = num + num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
			return optostr(num3.__get_op()) == "466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
		}

		virtual bool test_addition_eq() {
			_type num="2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
			_type num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			num += num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
			return optostr(num.__get_op()) == "466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
		}

		virtual bool test_subtraction() {
			// python3	SUBTRACTION TEST, CORRECT for all cases:
			/// hex(0x134395843534503845740957390690572-0x2337616833552046603458334740849159417653411302789319245661 + (1 << 256))
			///'0xffffffdcc89e97ccaadfb99fcba7ccb9f3b4c6ea11ceb043443e1ce07744af11'
			_type num="134395843534503845740957390690572"; // 132-bit hex
			_type num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			_type num3 = num - num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 233761683355204660345833460c4b3915ee314fbcbbc1e31f88bb50ef";
			return optostr(num3.__get_op()) == "233761683355204660345833460c4b3915ee314fbcbbc1e31f88bb50ef";
		}

		virtual bool test_subtraction_eq() {
			_type num="134395843534503845740957390690572"; // 132-bit hex
			_type num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			num2 -= num;
			std::cout << std::hex << std::endl << "			   output: " << num2;
			std::cout << std::endl << "the correct answer: 233761683355204660345833460c4b3915ee314fbcbbc1e31f88bb50ef";
			return optostr(num2.__get_op()) == "233761683355204660345833460c4b3915ee314fbcbbc1e31f88bb50ef";
		}

		virtual bool test_mul() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			_type num2="2000";
			_type num3 = num * num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 11320a8fe11090c25cf98f1109ca80baf894f6c6a6c64834dfd346be2d5d0";
			return optostr(num3.__get_op()) == "11320a8fe11090c25cf98f1109ca80baf894f6c6a6c64834dfd346be2d5d0";
		}

		virtual bool test_mul_eq() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			_type num2="2000"; // 132-bit hex
			num *= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 11320a8fe11090c25cf98f1109ca80baf894f6c6a6c64834dfd346be2d5d0";
			return optostr(num.__get_op()) == "11320a8fe11090c25cf98f1109ca80baf894f6c6a6c64834dfd346be2d5d0";
		}

		virtual bool test_div() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			_type num2="2000309429034920423834902";
			_type num3 = num / num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 119b95f9328dfcca5df0870060df27aaf0";
			return optostr(num3.__get_op()) == "119b95f9328dfcca5df0870060df27aaf0";
		}

		virtual bool test_div_eq() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			_type num2="2000309429034920423834902";
			num /= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 119b95f9328dfcca5df0870060df27aaf0";
			return optostr(num.__get_op()) == "119b95f9328dfcca5df0870060df27aaf0";
		}

		virtual bool test_mod() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			_type num2="23049";
			_type num3 = num % num2;
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: 90926";
			return optostr(num3.__get_op()) == "90926";
		}

		virtual bool test_mod_eq() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			_type num2="23049";
			num /= num2;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 90926";
			return optostr(num.__get_op()) == "90926";
		}

		virtual bool test_pow() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			_type num2="2";
			_type num3 = pow(num, num2);
			std::cout << std::hex << std::endl << "			   output: " << num3;
			std::cout << std::endl << "the correct answer: b9fe143015ff9a71ef6f1d05068d2b9a565f8dbb28b4c8bdb2b2416d5a6d50c1 ";
			return optostr(num3.__get_op()) == "b9fe143015ff9a71ef6f1d05068d2b9a565f8dbb28b4c8bdb2b2416d5a6d50c1";
		}

		/////////// TEST INCREMENT AND DECREMENT
		virtual bool test_inc() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			num++;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 2337616833552046603458334740849159417653411302789319245662";
			return optostr(num.__get_op()) == "2337616833552046603458334740849159417653411302789319245662";
		}

		virtual bool test_dec() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			num--;
			std::cout << std::hex << std::endl << "			   output: " << num;
			std::cout << std::endl << "the correct answer: 2337616833552046603458334740849159417653411302789319245660";
			return optostr(num.__get_op()) == "2337616833552046603458334740849159417653411302789319245660";
		}

		/////////// TEST INDEX

		virtual bool test_index_bit() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			bool first_index = num[(uint32_t)0];
			std::cout << std::endl << "the correct answer (index 0): 1";
			return first_index == 1;
		}

		virtual bool test_index_uint64() {
			_type num = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
			uint64_t first_index = num[(bitsize_t)0];
			return first_index == 0x2337616833ULL;
		}
};

// Benchmark test speed of Bigints
template<typename _type, typename bitsize_t, bitsize_t bitsize>
class Benchmark
{
	public:
    	static constexpr const __uint128_t num_long_int = ((__uint128_t)0xabcde0123456789fU<<64)|0xffffffffffffffffU;
    	static constexpr const __uint128_t num_long_int2 = ((__uint128_t)0xffffffffffffffffU<<64)|0xffffffffffffffffU;

		_type number = "2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
		_type number2 = "2337616833552046603458334740849159417653411302789319245660"; // 232-bit hex
		_type zero = "0";
		_type one = "1";
		_type two = "2";
		_type rand = "2043835430954379549087549803752094857685945098653096065479"; // 232-bit hex
		_type and_ex = "13439085734205734"; // 232-bit hex
		_type example = "4423905842305789076053609538769048324785042058710953420857"; // 232-bit hex
		_type arithmetic_ex = "134395843534503845740957390690572"; // 232-bit hex
		_type _2000 ="2000"; // mul
		_type div_ex = "2000309429034920423834902"; // div
		_type _23049 = "23049"; // mod

		Benchmark() = default;

		virtual void reset() {
				randomize();
		}
		static constexpr const char hex_digits[17] = "0123456789abcdef";

		std::string rand_str(bitsize_t _bitsize)
		{
		    // random byte using Mersenne Twister, generate random bitset length, then generate a random string of that random length

    		std::random_device randDev;
    		std::mt19937 generator(randDev() ^ time(NULL));
    		std::uniform_int_distribution<bitsize_t> distr(0, _bitsize);
    		std::uniform_int_distribution<uint8_t> distr2(0, 16);
			bitsize_t length = distr(generator);
			bitsize_t hexlen = length%4==0 ? length/4 : length/4+1;
			std::string *str = new std::string;
			for(bitsize_t i=0;i<hexlen;i++) {
				*str += hex_digits[distr2(generator)];
			}
			std::string ret = *str;
			delete str;
			return ret;
		}

		virtual void randomize() {
			_type number = rand_str(bitsize); // 232-bit hex
			_type number2 = rand_str(bitsize); // 232-bit hex
			_type rand = rand_str(bitsize); // 232-bit hex
			_type and_ex = rand_str(bitsize); // 232-bit hex
			_type example = rand_str(bitsize); // 232-bit hex
			_type arithmetic_ex = rand_str(bitsize); // 232-bit hex
			_type _2000 =rand_str(bitsize);
			_type div_ex = rand_str(bitsize);
			_type _23049 = rand_str(bitsize);
		}

	// assignment operators (7 functions)
	// test_chararr_assignment, test_str_assignment, test_long_chararr_assignment, test_long_str_assignment, test_long_hex_assignment, test_int_assignment, test_long_int_assignment

		/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT, HEX)
		virtual void test_chararr_assignment() {
			_type num="256";
		}
		
		virtual void test_str_assignment() {
			_type num = std::string("256");
		}
		
		virtual void test_long_chararr_assignment() {
			_type num = "56539106072908298546665520023773392506479484700019806659891398441363832832";
		}
		
		virtual void test_long_str_assignment() {
			_type num = std::string("56539106072908298546665520023773392506479484700019806659891398441363832832");
		}
		
		virtual void test_long_hex_assignment() {
			_type num = std::string("0x5653910607290829E8546665D5A200237733925064794847000198066598913984FF41363832B832F");
		}

		/////////// TEST STRING AND CHAR ASSIGNMENT OPERATORS (INT (amount of variadic parameters), INT)
		virtual void test_int_assignment() {
			_type num = _type(1, 256);
		}
		
		// test variadic constructor of type __uint128_t
		virtual void test_long_int_assignment() {
			_type num = _type(2, num_long_int, num_long_int2);
		}

	// boolean operators (9 functions)
	// test_bool_and, test_bool_or, test_bool_equal_eq, test_bool_not, test_bool_not_eq, test_bool_less, test_bool_less_eq, test_bool_greater, test_bool_greater_eq
	virtual void test_bool_not_eq() {
		bool ret = number != rand;
	}

	virtual void test_bool_not() {
		bool ret = !number;
	}

	virtual void test_bool_less() {
		bool ret = number2 < number;
	}

	virtual void test_bool_less_eq() {
		bool ret = number2 <= number;
	}

	virtual void test_bool_and() {
		bool ret = number2 && one; // true, because both are 1 or bigger
	}

	virtual void test_bool_or() {
		bool ret = number2 || zero; // true, because num is 1 or bigger
	}

	virtual void test_bool_equal_eq() {
		bool ret = number2 == number2; // true, because they are equal
		
	}

	virtual void test_bool_greater() {
		bool ret = number > number2; // true, because num is bigger
	}

	virtual void test_bool_greater_eq() {
		bool ret = number >= number2; // true, because num is bigger
	}

	// bitwise operators (11 functions)
	// test_bitwise_not, bitwise_and, bitwise_and_eq, bitwise_xor, bitwise_xor_eq, bitwise_rshift,
	// bitwise_rshift_eq, bitwise_lshift, bitwise_lshift_eq, bitwise_or, bitwise_or_eq
		virtual void test_bitwise_not() {
			_type num2 = ~number;
		}
		virtual void test_bitwise_and() {
			_type num3 = number & and_ex; // 232-bit hex
			
		}
		void test_bitwise_and_eq() {
			number &= and_ex;
		}
		virtual void test_bitwise_xor() {
			_type num3 = number ^ example;
		}
		virtual void test_bitwise_xor_eq() {
			number ^= example;
		}
		virtual void test_bitwise_rshift() {
			_type num2 = number >> _type(2);
			
		}
		virtual void test_bitwise_rshift_eq() {
			number >>= _type(2);
			
		}
		virtual void test_bitwise_lshift() {
			_type num2 = number << _type(20);
		}	
		virtual void test_bitwise_lshift_eq() {
			number <<= 20;
		}
		virtual void test_bitwise_or() {
			_type num3 = number | example;
		}
		virtual void test_bitwise_or_eq() {
			number |= example;
		}	

	// basic arithmetic (11 functions)
	// test_addition, test_addition_eq, test_subtraction, test_subtraction_eq, test_mul, test_mul_eq, test_div, test_div_eq, test_mod, test_mod_eq, test_pow
	virtual void test_addition() {
		_type num3 = number + number;
	}

	virtual void test_addition_eq() {
		number += number;
	}

	virtual void test_subtraction() {
		// python3	SUBTRACTION TEST, CORRECT for all cases:
		/// hex(0x134395843534503845740957390690572-0x2337616833552046603458334740849159417653411302789319245661 + (1 << 256))
		///'0xffffffdcc89e97ccaadfb99fcba7ccb9f3b4c6ea11ceb043443e1ce07744af11'
		_type num3 = number - arithmetic_ex;
	}

	virtual void test_subtraction_eq() {
		number -= arithmetic_ex;
	}

	virtual void test_mul() {
		_type num3 = number * _2000;
	}

	virtual void test_mul_eq() {
		number *= _2000;
	}

	virtual void test_div() {
		_type num3 = number / div_ex;
	}

	virtual void test_div_eq() {
		number /= div_ex;
	}

	virtual void test_mod() {
		_type num3 = number % _23049;
	}

	virtual void test_mod_eq() {
		number %= _23049;
	}

	virtual void test_pow() {
		_type num3 = pow(number, two);
	}

	// incrementation and decrementation (2 functions)
	// test_inc, test_dec
	virtual void test_inc() {
		number++;
	}

	virtual void test_dec() {
		number--;
	}

	// index operators (/2 functions)
	// test_index_bit, test_index_uint64
		virtual void test_index_bit() {
			number.isbit = 1;
			bool first_index = number[0];
		}

		virtual void test_index_uint64() {
			number.isbit = 0;
			uint64_t first_index = number[0];
		}

	std::chrono::high_resolution_clock::time_point timer_starts[42];
	std::chrono::high_resolution_clock::time_point timer_ends[42];
	void benchmark_once()
	{
		// test_chararr_assignment, test_str_assignment, test_long_chararr_assignment, test_long_str_assignment, test_long_hex_assignment, test_int_assignment, test_long_int_assignment
		timer_starts[0] = std::chrono::high_resolution_clock::now();
		test_chararr_assignment();
		timer_ends[0] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[1] = std::chrono::high_resolution_clock::now();
		test_str_assignment();
		timer_ends[1] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[2] = std::chrono::high_resolution_clock::now();
		test_long_chararr_assignment();
		timer_ends[2] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[3] = std::chrono::high_resolution_clock::now();
		test_long_str_assignment();
		timer_ends[3] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[4] = std::chrono::high_resolution_clock::now();
		test_long_hex_assignment();
		timer_ends[4] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[5] = std::chrono::high_resolution_clock::now();
		test_int_assignment();
		timer_ends[5] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[6] = std::chrono::high_resolution_clock::now();
		test_long_int_assignment();
		timer_ends[6] = std::chrono::high_resolution_clock::now();
		reset();
	
		// test_bool_and, test_bool_or, test_bool_equal_eq, test_bool_not, test_bool_not_eq, test_bool_less, test_bool_less_eq, test_bool_greater, test_bool_greater_eq
		timer_starts[7] = std::chrono::high_resolution_clock::now();
		test_bool_and();
		timer_ends[7] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[8] = std::chrono::high_resolution_clock::now();
		test_bool_or();
		timer_ends[8] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[9] = std::chrono::high_resolution_clock::now();
		test_bool_equal_eq();
		timer_ends[9] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[10] = std::chrono::high_resolution_clock::now();
		test_bool_not();
		timer_ends[10] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[11] = std::chrono::high_resolution_clock::now();
		test_bool_not_eq();
		timer_ends[11] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[12] = std::chrono::high_resolution_clock::now();
		test_bool_less();
		timer_ends[12] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[13] = std::chrono::high_resolution_clock::now();
		test_bool_less_eq();
		timer_ends[13] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[14] = std::chrono::high_resolution_clock::now();
		test_bool_greater();
		timer_ends[14] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[15] = std::chrono::high_resolution_clock::now();
		test_bool_greater_eq();
		timer_ends[15] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[16] = std::chrono::high_resolution_clock::now();
		test_bool_less_eq();
		timer_ends[16] = std::chrono::high_resolution_clock::now();
		reset();
	
		// test_bitwise_not, bitwise_and, bitwise_and_eq, bitwise_xor, bitwise_xor_eq, bitwise_rshift,
		// bitwise_rshift_eq, bitwise_lshift, bitwise_lshift_eq, bitwise_or, bitwise_or_eq
		timer_starts[17] = std::chrono::high_resolution_clock::now();
		test_bitwise_not();
		timer_ends[17] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[18] = std::chrono::high_resolution_clock::now();
		test_bitwise_and();
		timer_ends[18] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[19] = std::chrono::high_resolution_clock::now();
		test_bitwise_and_eq();
		timer_ends[19] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[20] = std::chrono::high_resolution_clock::now();
		test_bitwise_xor();
		timer_ends[20] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[21] = std::chrono::high_resolution_clock::now();
		test_bitwise_xor_eq();
		timer_ends[21] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[22] = std::chrono::high_resolution_clock::now();
		test_bitwise_rshift();
		timer_ends[22] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[22] = std::chrono::high_resolution_clock::now();
		test_bitwise_rshift_eq();
		timer_ends[22] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[23] = std::chrono::high_resolution_clock::now();
		test_bitwise_lshift();
		timer_ends[23] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[24] = std::chrono::high_resolution_clock::now();
		test_bitwise_lshift_eq();
		timer_ends[24] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[25] = std::chrono::high_resolution_clock::now();
		test_bitwise_or();
		timer_ends[25] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[26] = std::chrono::high_resolution_clock::now();
		test_bitwise_or_eq();
		timer_ends[26] = std::chrono::high_resolution_clock::now();
		reset();
	
		// test_addition, test_addition_eq, test_subtraction, test_subtraction_eq, test_mul, test_mul_eq, test_div, test_div_eq, test_mod, test_mod_eq, test_pow
		timer_starts[27] = std::chrono::high_resolution_clock::now();
		test_addition();
		timer_ends[27] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[28] = std::chrono::high_resolution_clock::now();
		test_addition_eq();
		timer_ends[28] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[29] = std::chrono::high_resolution_clock::now();
		test_subtraction();
		timer_ends[29] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[30] = std::chrono::high_resolution_clock::now();
		test_subtraction_eq();
		timer_ends[30] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[31] = std::chrono::high_resolution_clock::now();
		test_mul();
		timer_ends[31] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[32] = std::chrono::high_resolution_clock::now();
		test_mul_eq();
		timer_ends[32] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[33] = std::chrono::high_resolution_clock::now();
		test_div();
		timer_ends[33] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[34] = std::chrono::high_resolution_clock::now();
		test_div_eq();
		timer_ends[34] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[35] = std::chrono::high_resolution_clock::now();
		test_mod();
		timer_ends[35] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[36] = std::chrono::high_resolution_clock::now();
		test_mod_eq();
		timer_ends[36] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[37] = std::chrono::high_resolution_clock::now();
		test_pow();
		timer_ends[37] = std::chrono::high_resolution_clock::now();
		reset();
	
		// test_inc, test_dec
		timer_starts[38] = std::chrono::high_resolution_clock::now();
		test_inc();
		timer_ends[38] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[39] = std::chrono::high_resolution_clock::now();
		test_dec();
		timer_ends[39] = std::chrono::high_resolution_clock::now();
		reset();
	
		// test_index_bit, test_index_uint64
		timer_starts[40] = std::chrono::high_resolution_clock::now();
		test_index_bit();
		timer_ends[40] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[41] = std::chrono::high_resolution_clock::now();
		test_index_uint64();
		timer_ends[41] = std::chrono::high_resolution_clock::now();
		reset();
	}
};

// uint64_t benchmark tests to compare to
template<typename _type>
class UintTBenchmark
{
	public:
		_type number;
		_type number2;
		_type zero;
		_type one;
		_type two;
		_type rand;
		_type and_ex;
		_type example;
		_type arithmetic_ex;
		_type _2000;
		_type div_ex;
		_type _23049;
		UintTBenchmark() {
			reset();

		}

		virtual void reset() {
			number        = randuT();
			number2       = randuT();
			rand          = randuT();
			and_ex        = randuT();
			example       = randuT();
			arithmetic_ex = randuT();
			_2000 		= randuT();
			div_ex		= randuT();
			_23049		= randuT();
			two		= randuT();
			one		= randuT();
			zero		= randuT();
		}

		uint64_t randuT()
		{
			if constexpr(sizeof(_type) == sizeof(__uint128_t)) {
    			std::random_device randDev;
    			std::mt19937 generator(randDev() ^ time(NULL));
    			std::uniform_int_distribution<uint64_t> distr;
				uint64_t num1 = distr(generator);
				uint64_t num2 = distr(generator);
				while(num1 == 0) num1 = distr(generator);
				while(num2 == 0) num2 = distr(generator);
				if constexpr(std::is_signed<_type>::value) {
					return ((__int128_t)num1 << 64) | num2;
				} else {
					return ((__uint128_t)num1 << 64) | num2;
				}
			} else {
    			std::random_device randDev;
    			std::mt19937 generator(randDev() ^ time(NULL));
    			std::uniform_int_distribution<_type> distr;
				_type num = distr(generator);
				while(num == 0) num = distr(generator);
				return num;
			}
		}

		virtual void test_int_assignment() {
			_type num = (_type)256;
		}

		// boolean operators (9 functions)
		// test_bool_and, test_bool_or, test_bool_equal_eq, test_bool_not, test_bool_not_eq, test_bool_less, test_bool_less_eq, test_bool_greater, test_bool_greater_eq
		virtual void test_bool_not_eq() {
			bool ret = number != rand;
		}

		virtual void test_bool_not() {
			bool ret = !number;
		}

		virtual void test_bool_less() {
			bool ret = number2 < number;
		}

		virtual void test_bool_less_eq() {
			bool ret = number2 <= number;
		}

		virtual void test_bool_and() {
			bool ret = number2 && one; // true, because both are 1 or bigger
		}

		virtual void test_bool_or() {
			bool ret = number2 || zero; // true, because num is 1 or bigger
		}

		virtual void test_bool_equal_eq() {
			bool ret = number2 == number2; // true, because they are equal
			
		}

		virtual void test_bool_greater() {
			bool ret = number > number2; // true, because num is bigger
		}

		virtual void test_bool_greater_eq() {
			bool ret = number >= number2; // true, because num is bigger
		}

		// bitwise operators (11 functions)
		// test_bitwise_not, bitwise_and, bitwise_and_eq, bitwise_xor, bitwise_xor_eq, bitwise_rshift,
		// bitwise_rshift_eq, bitwise_lshift, bitwise_lshift_eq, bitwise_or, bitwise_or_eq
			virtual void test_bitwise_not() {
				_type num2 = ~number;
			}
			virtual void test_bitwise_and() {
				_type num3 = number & and_ex; // 232-bit hex
				
			}
			void test_bitwise_and_eq() {
				number &= and_ex;
			}
			virtual void test_bitwise_xor() {
				_type num3 = number ^ example;
			}
			virtual void test_bitwise_xor_eq() {
				number ^= example;
			}
			virtual void test_bitwise_rshift() {
				_type num2 = number >> _type(2);
				
			}
			virtual void test_bitwise_rshift_eq() {
				number >>= _type(2);
				
			}
			virtual void test_bitwise_lshift() {
				_type num2 = number << _type(7);
			}	
			virtual void test_bitwise_lshift_eq() {
				number <<= 7;
			}
			virtual void test_bitwise_or() {
				_type num3 = number | example;
			}
			virtual void test_bitwise_or_eq() {
				number |= example;
			}	

		// basic arithmetic (11 functions)
		// test_addition, test_addition_eq, test_subtraction, test_subtraction_eq, test_mul, test_mul_eq, test_div, test_div_eq, test_mod, test_mod_eq, test_pow
		virtual void test_addition() {
			_type num3 = number + number;
		}

		virtual void test_addition_eq() {
			number += number;
		}

		virtual void test_subtraction() {
			// python3	SUBTRACTION TEST, CORRECT for all cases:
			/// hex(0x134395843534503845740957390690572-0x2337616833552046603458334740849159417653411302789319245661 + (1 << 256))
			///'0xffffffdcc89e97ccaadfb99fcba7ccb9f3b4c6ea11ceb043443e1ce07744af11'
			_type num3 = number - arithmetic_ex;
		}

		virtual void test_subtraction_eq() {
			number -= arithmetic_ex;
		}

		virtual void test_mul() {
			_type num3 = number * _2000;
		}

		virtual void test_mul_eq() {
			number *= _2000;
		}

		virtual void test_div() {
			_type num3 = number / div_ex;
		}

		virtual void test_div_eq() {
			number /= div_ex;
		}

		virtual void test_mod() {
			_type num3 = number % _23049;
		}

		virtual void test_mod_eq() {
			number %= _23049;
		}

		virtual void test_pow() {
			_type num3 = pow(number, two);
		}

		// incrementation and decrementation (2 functions)
		// test_inc, test_dec
		virtual void test_inc() {
			number++;
		}

		virtual void test_dec() {
			number--;
		}

	std::chrono::high_resolution_clock::time_point timer_starts[34];
	std::chrono::high_resolution_clock::time_point timer_ends[34];
	void benchmark_once()
	{
		// test_chararr_assignment, test_str_assignment, test_long_chararr_assignment, test_long_str_assignment, test_long_hex_assignment, test_int_assignment, test_long_int_assignment
		timer_starts[0] = std::chrono::high_resolution_clock::now();
		test_int_assignment();
		timer_ends[0] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[1] = std::chrono::high_resolution_clock::now();
		test_bool_and();
		timer_ends[1] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[2] = std::chrono::high_resolution_clock::now();
		test_bool_or();
		timer_ends[2] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[3] = std::chrono::high_resolution_clock::now();
		test_bool_equal_eq();
		timer_ends[3] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[4] = std::chrono::high_resolution_clock::now();
		test_bool_not();
		timer_ends[4] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[5] = std::chrono::high_resolution_clock::now();
		test_bool_not_eq();
		timer_ends[5] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[6] = std::chrono::high_resolution_clock::now();
		test_bool_less();
		timer_ends[6] = std::chrono::high_resolution_clock::now();
		reset();
	
		// test_bool_and, test_bool_or, test_bool_equal_eq, test_bool_not, test_bool_not_eq, test_bool_less, test_bool_less_eq, test_bool_greater, test_bool_greater_eq
		timer_starts[7] = std::chrono::high_resolution_clock::now();
		test_bool_less_eq();
		timer_ends[7] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[8] = std::chrono::high_resolution_clock::now();
		test_bool_greater();
		timer_ends[8] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[9] = std::chrono::high_resolution_clock::now();
		test_bool_greater_eq();
		timer_ends[9] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[10] = std::chrono::high_resolution_clock::now();
		test_bool_less_eq();
		timer_ends[10] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[11] = std::chrono::high_resolution_clock::now();
		test_bitwise_not();
		timer_ends[11] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[12] = std::chrono::high_resolution_clock::now();
		test_bitwise_and();
		timer_ends[12] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[13] = std::chrono::high_resolution_clock::now();
		test_bitwise_and_eq();
		timer_ends[13] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[14] = std::chrono::high_resolution_clock::now();
		test_bitwise_xor();
		timer_ends[14] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[15] = std::chrono::high_resolution_clock::now();
		test_bitwise_xor_eq();
		timer_ends[15] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[16] = std::chrono::high_resolution_clock::now();
		test_bitwise_rshift();
		timer_ends[16] = std::chrono::high_resolution_clock::now();
		reset();
	
		// test_bitwise_not, bitwise_and, bitwise_and_eq, bitwise_xor, bitwise_xor_eq, bitwise_rshift,
		// bitwise_rshift_eq, bitwise_lshift, bitwise_lshift_eq, bitwise_or, bitwise_or_eq
		timer_starts[17] = std::chrono::high_resolution_clock::now();
		test_bitwise_rshift_eq();
		timer_ends[17] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[18] = std::chrono::high_resolution_clock::now();
		test_bitwise_lshift();
		timer_ends[18] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[19] = std::chrono::high_resolution_clock::now();
		test_bitwise_lshift_eq();
		timer_ends[19] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[20] = std::chrono::high_resolution_clock::now();
		test_bitwise_or();
		timer_ends[20] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[21] = std::chrono::high_resolution_clock::now();
		test_bitwise_or_eq();
		timer_ends[21] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[22] = std::chrono::high_resolution_clock::now();
		test_addition();
		timer_ends[22] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[22] = std::chrono::high_resolution_clock::now();
		test_addition_eq();
		timer_ends[22] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[23] = std::chrono::high_resolution_clock::now();
		test_subtraction();
		timer_ends[23] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[24] = std::chrono::high_resolution_clock::now();
		test_subtraction_eq();
		timer_ends[24] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[25] = std::chrono::high_resolution_clock::now();
		test_mul();
		timer_ends[25] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[26] = std::chrono::high_resolution_clock::now();
		test_mul_eq();
		timer_ends[26] = std::chrono::high_resolution_clock::now();
		reset();
	
		// test_addition, test_addition_eq, test_subtraction, test_subtraction_eq, test_mul, test_mul_eq, test_div, test_div_eq, test_mod, test_mod_eq, test_pow
		timer_starts[27] = std::chrono::high_resolution_clock::now();
		test_div();
		timer_ends[27] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[28] = std::chrono::high_resolution_clock::now();
		test_div_eq();
		timer_ends[28] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[29] = std::chrono::high_resolution_clock::now();
		test_mod();
		timer_ends[29] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[30] = std::chrono::high_resolution_clock::now();
		test_mod_eq();
		timer_ends[30] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[31] = std::chrono::high_resolution_clock::now();
		test_pow();
		timer_ends[31] = std::chrono::high_resolution_clock::now();
		reset();
	
		// test_inc, test_dec
		timer_starts[32] = std::chrono::high_resolution_clock::now();
		test_inc();
		timer_ends[32] = std::chrono::high_resolution_clock::now();
		reset();
	
		timer_starts[33] = std::chrono::high_resolution_clock::now();
		test_dec();
		timer_ends[33] = std::chrono::high_resolution_clock::now();
		reset();
	}
	
};

template<typename _type>
uint64_t *benchmark_tests_T(uint64_t &total_time, uint32_t count=100)
{
	uint16_t size = sizeof(UintTBenchmark<_type>::timer_starts)/
  				 		   sizeof(UintTBenchmark<_type>::timer_starts[0]);
	uint64_t *average_timeT = new uint64_t[size];

	// assign times to zero
	for(uint16_t j=0;j<size;j++) {
		average_timeT[j] = 0;
	}

	for(uint32_t i=0;i<count;i++) {
		UintTBenchmark<_type> benchmarker = UintTBenchmark<_type>();
		benchmarker.benchmark_once();
		for(uint16_t j=0;j<size;j++) {
			auto timer = (benchmarker.timer_ends[j]-benchmarker.timer_starts[j]); // divide by count to get average
			average_timeT[j] += timer.count();
		}
	}

	for(uint16_t j=0;j<size;j++) {
		average_timeT[j] /= count;
	}
	std::string all_functions[34] = {"test_int_assignment =", "test_bool_and &&", "test_bool_or ||", "test_bool_equal_eq ==", "test_bool_not !", "test_bool_not_eq !=", "test_bool_less <", "test_bool_less_eq <=", "test_bool_greater >", "test_bool_greater_eq >=", "test_bitwise_not ~", "bitwise_and &", "bitwise_and_eq &=", "bitwise_xor ^", "bitwise_xor_eq ^=", "bitwise_rshift >>", "bitwise_rshift_eq >>=", "bitwise_lshift <<", "bitwise_lshift_eq <<=", "bitwise_or |", "bitwise_or_eq |=", "test_addition +", "test_addition_eq +=", "test_subtraction -", "test_subtraction_eq -=", "test_mul *", "test_mul_eq *=", "test_div /", "test_div_eq /=", "test_mod %", "test_mod_eq %=", "test_pow pow()", "test_inc ++", "test_dec --"};

	total_time = 0;
	for(uint16_t i=0;i<size;i++) {
		auto timer = std::chrono::nanoseconds(average_timeT[i]); // divide by count to get average
  		std::chrono::microseconds micro = std::chrono::duration_cast< std::chrono::microseconds >( timer );
  		std::chrono::milliseconds mili = std::chrono::duration_cast< std::chrono::milliseconds >( timer );
  		std::chrono::seconds second = std::chrono::duration_cast< std::chrono::seconds >( timer );
		total_time+=timer.count();

		std::cout << "\nfunction: " << all_functions[i];
		std::cout << "\ntimer = \t" << timer << "\t:\t" << micro << "\t:\t" << mili << "\t:\t" << second << "\t|" << std::endl;
	}
	std::cout << std::endl << "Total time: " << total_time << "ns" << std::endl;
	return average_timeT;
}

int main()
{
	typedef uint32_t bitsize_t;
	constexpr const static bitsize_t bitsize = 512; // 33554432; // 2147483648;
	typedef LargeUint<bitsize> biguint_t;
	static uint32_t count=1;

	/************* VALUE TESTS *************/
	// Test<biguint_t, bitsize_t, bitsize> tester = Test<biguint_t, bitsize_t, bitsize>();
	// value tests can be done at your own pace, test a function by calling tester.<function-name>().
	
	std::string all_functions[42] = {"test_chararr_assignment =", "test_str_assignment =", "test_long_chararr_assignment =", "test_long_str_assignment =", "test_long_hex_assignment =", "test_int_assignment =", "test_long_int_assignment =", "test_bool_and &&", "test_bool_or ||", "test_bool_equal_eq ==", "test_bool_not !", "test_bool_not_eq !=", "test_bool_less <", "test_bool_less_eq <=", "test_bool_greater >", "test_bool_greater_eq >=", "test_bitwise_not ~", "bitwise_and &", "bitwise_and_eq &=", "bitwise_xor ^", "bitwise_xor_eq ^=", "bitwise_rshift >>", "bitwise_rshift_eq >>=", "bitwise_lshift <<", "bitwise_lshift_eq <<=", "bitwise_or |", "bitwise_or_eq |=", "test_addition +", "test_addition_eq +=", "test_subtraction -", "test_subtraction_eq -=", "test_mul *", "test_mul_eq *=", "test_div /", "test_div_eq /=", "test_mod %", "test_mod_eq %=", "test_pow pow()", "test_inc ++", "test_dec --", "test_index_bit []", "test_index_uint64 []"};

	/************* BENCHMARK TESTS *************/

	uint64_t average_time[sizeof(Benchmark<biguint_t, bitsize_t, bitsize>::timer_starts)/
  				 				 sizeof(Benchmark<biguint_t, bitsize_t, bitsize>::timer_starts[0])];

	uint64_t int8_time, int16_time, int32_time, int64_time, int128_time,
			 uint8_time, uint16_time, uint32_time, uint64_time, uint128_time;

	// size of average time int pointers
	uint16_t size = sizeof(UintTBenchmark<int8_t>::timer_starts)/
  				 		   sizeof(UintTBenchmark<int8_t>::timer_starts[0]);
	std::cout << "\n\n---------------------\nAVERAGE TIME FOR int8:\n---------------------\n";
	uint64_t *average_time_int8  = benchmark_tests_T<int8_t>(int8_time, count);
	std::cout << "\n\n----------------------\nAVERAGE TIME FOR int16:\n----------------------\n";
	uint64_t *average_time_int16 = benchmark_tests_T<int16_t>(int16_time, count);
	std::cout << "\n\n----------------------\nAVERAGE TIME FOR int32:\n----------------------\n";
	uint64_t *average_time_int32 = benchmark_tests_T<int32_t>(int32_time, count);
	std::cout << "\n\n----------------------\nAVERAGE TIME FOR int64:\n----------------------\n";
	uint64_t *average_time_int64 = benchmark_tests_T<int64_t>(int64_time, count);
	std::cout << "\n\n-----------------------\nAVERAGE TIME FOR int128:\n-----------------------\n";
	uint64_t *average_time_int128 = benchmark_tests_T<__int128_t>(int128_time, count);

	std::cout << "\n\n----------------------\nAVERAGE TIME FOR uint8:\n----------------------\n";
	uint64_t *average_time_uint8  = benchmark_tests_T<uint8_t>(uint8_time, count);
	std::cout << "\n\n-----------------------\nAVERAGE TIME FOR uint16:\n-----------------------\n";
	uint64_t *average_time_uint16 = benchmark_tests_T<uint16_t>(uint16_time, count);
	std::cout << "\n\n-----------------------\nAVERAGE TIME FOR uint32:\n-----------------------\n";
	uint64_t *average_time_uint32 = benchmark_tests_T<uint32_t>(uint32_time, count);
	std::cout << "\n\n-----------------------\nAVERAGE TIME FOR uint64:\n-----------------------\n";
	uint64_t *average_time_uint64 = benchmark_tests_T<uint64_t>(uint64_time, count);
	std::cout << "\n\n------------------------\nAVERAGE TIME FOR uint128:\n------------------------\n";
	uint64_t *average_time_uint128 = benchmark_tests_T<__uint128_t>(uint128_time, count);

	// assign times to zero
	for(uint16_t j=0;j<sizeof(average_time)/sizeof(average_time[0]);j++) {
		average_time[j] = 0;
	}
	for(uint32_t i=0;i<count;i++) {
		Benchmark<biguint_t, bitsize_t, bitsize> benchmarker = Benchmark<biguint_t, bitsize_t, bitsize>();
		benchmarker.benchmark_once();
		benchmarker.randomize();
		for(uint16_t j=0;j<sizeof(average_time)/sizeof(average_time[0]);j++) {
			auto timer = (benchmarker.timer_ends[j]-benchmarker.timer_starts[j]); // divide by count to get average
			average_time[j] += timer.count();
		}
	}

	uint64_t total_time_bench = 0;
	for(uint16_t i=0;i<sizeof(average_time)/sizeof(average_time[0]);i++) {
		auto timer = std::chrono::nanoseconds(average_time[i]/count); // divide by count to get average
  		std::chrono::microseconds micro = std::chrono::duration_cast< std::chrono::microseconds >( timer );
  		std::chrono::milliseconds mili = std::chrono::duration_cast< std::chrono::milliseconds >( timer );
  		std::chrono::seconds second = std::chrono::duration_cast< std::chrono::seconds >( timer );
		total_time_bench+=timer.count();

		std::cout << "\nfunction: " << all_functions[i];
		std::cout << "\ntimer = \t" << timer << "\t:\t" << micro << "\t:\t" << mili << "\t:\t" << second << "\t|";

		if(i == 7) i++;
		// compare to uint64 proportionally, meaning if 512-bits / 64-bits then compare the ratio
		// if(i > 5 and i != 7 and i <40) std::cout << "\tuint64: " << average_time_uint64[i-6] << "\tbig/u64: " << ((double)timer.count()/(bitsize/64))/average_time_uint64[i-5];
		// compare to uint64 upproportionally
		if(i >= 5 and i != 6 and i <40) std::cout << "\tuint64: " << average_time_uint64[i-6] << "ns\tbig/u64: " << (double)timer.count()/average_time_uint64[i-6];
		std::cout << std::endl;
	}
	std::cout << std::endl << "Total time: " << total_time_bench/1000 << "µs" << std::endl;

	delete[] average_time_int8;
	delete[] average_time_int16;
	delete[] average_time_int32;
	delete[] average_time_int64;
	delete[] average_time_int128;

	delete[] average_time_uint8;
	delete[] average_time_uint16;
	delete[] average_time_uint32;
	delete[] average_time_uint64;
	delete[] average_time_uint128;
	return 0;
}

#endif /* TEST_CPP*/
