#ifndef BIGINT_CPP
#define BIGINT_CPP

#include <iostream>
#include <string>
#include <stdint.h>
#include <cstdarg>

#include "bigint.h"

namespace BigInt
{
	template<uint16_t bitsize>
	template<uint8_t base> // type of input (oct = base 8, hex = base 16)
	BigUint<bitsize>::BigUint(std::string input)
	{
		strtobigint<base>(input.c_str());
	}
	
	template<uint16_t bitsize>
	template<uint8_t base> // type of input (oct = base 8, hex = base 16)
	BigUint<bitsize>::BigUint(const char* input)
	{
		strtobigint<base>(input); // to avoid annoying C++ conversion error
	}
	
	// numerical input. If number is 256-bit, input = left 128-bit, right 128-bit
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpragmas"
	#pragma GCC diagnostic ignored "-Wc++2b-extensions"
	#pragma GCC diagnostic ignored "-Wvarargs"
	template<uint16_t bitsize> 
	constexpr BigUint<bitsize>::BigUint(const uint16_t count, __uint128_t ...) {
	    // uint128_t input to 2 uint64_t integers
		// constant mask values
	    static constexpr const __uint128_t bottom_mask = (__uint128_t{1} << 64) - 1; // 0x0000000000000000ffffffffffffffffU
	    static constexpr const __uint128_t top_mask = ~bottom_mask;                  // 0xffffffffffffffff0000000000000000U

		std::va_list args;
		va_start(args, count);
	
		// pad the operator array
		const uint16_t count64 = count << 1; // count if input is 64-bits
		op_nonleading_i = count64 < op_size? op_size-count64 : 0;
		for(uint16_t i=0;i<=op_nonleading_i;i++) op[i] = 0x0000000000000000ULL;
	
		// add the inputs to the operator array
	    for(size_t i=0;i<count;i++) {
	        __uint128_t num = va_arg(args, __uint128_t);
	        op[op_size-i*2-1] = num&bottom_mask;
	        op[op_size-i*2-2] = (num>>64)&top_mask;
	    }
		va_end(args);
	}
	#pragma GCC diagnostic pop
	
	// assignment to operator array of known length
	template<uint16_t bitsize>
	BigUint<bitsize>::BigUint(uint64_t *input, uint16_t len) // input order has to be: input[0] = most left 64-bit
	{
		// calculate pad count
		uint16_t pad_count;
		if(len > op_size) { // if bigger integer type, and input array has non-zero number at the height of op
			bool found = false;
			for(uint16_t i=0;i<=op_size;i++) { // iterate to op_size because input array order is backwards
				if(input[i] != 0)
					found = true;
			}
			if(found) { // maybe input[op_size] = 0 but input[op_size+1] !=0, therefore use the found defined above
				throw int_too_large_error(("given integer is too large for the defined BigUint<" + std::to_string(bitsize) +
										  "> which is not enough to hold a value of BigUint<" + std::to_string(len*64) + ">").c_str());
			} else {
				uint16_t nonzeroi; // index of non-zero input element
				for(uint16_t i=op_size+1;i<len;i++) { // basically continuation of previous loop
					if(input[i] != 0) {
						nonzeroi=i;
						break;
					}
				}
	    		pad_count = nonzeroi;
			}
		} else {
			pad_count = len-op_size;
	
		}
	    // pad the operator array
	    for(uint16_t i=0;i<pad_count;i++) op[i] = 0x0000000000000000ULL;
	
	    // add input to operator array
	    for(uint16_t i=op_size;i --> pad_count;) op[i] = input[i];
		op_nonleading_i = pad_count;
	}
	
	// TODO: recreate the above function with len as template parameter and as constexpr function
	
	//#pragma GCC diagnostic push
	//#pragma GCC diagnostic ignored "-Wtype-limits"
	//template<uint16_t bitsize>
	//BigUint<bitsize> BigUint<bitsize>::operator=(const BigUint &num)
	//{
	//	// assign non-leading-zero elements of the operator array
	//	if(num.op_size <= op_size) {
	//		for(uint16_t i=0;i<num.op_size;i++) op[i] = num.op[i];
	//		for(uint16_t i=num.op_size;i<op_size;i++) op[i] = num.op[i]; // op_size is bigger than num.op_size, pad op
	//	} else {
	//		for(uint16_t i=num.op_size;i>0;i++) op[i] = num.op[i];
	//		for(uint16_t i=num.op_size;i<op_size;i++) op[i] = num.op[i];
	//	}
	//	return *this;
	//}
	//#pragma GCC diagnostic pop
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint assignment operator")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator=(const char* &num)
	{
		delete this; // object suicide
		return BigUint<bitsize>(num); // reconstruct as new object
	}
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint boolean and operator&&")]]
	constexpr bool BigUint<bitsize>::operator&&(const BigUint &num)
	{
		if (*this == 0 or num == 0) return 0;
		return 1;
	}
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint boolean or operator||")]]
	constexpr bool BigUint<bitsize>::operator||(const BigUint &num)
	{
		if (*this == 0 and num == 0) return 0;
		return 1;
	}
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint boolean equal to operator==")]]
	constexpr bool BigUint<bitsize>::operator==(const BigUint &num)
	{
		bool equal = 0;
		if(op_size >= num.op_size) {
			for(uint16_t i=num.op_size;i<op_size && !equal;i++) equal |= op[i] == 0;
			for(uint16_t i=0;i<num.op_size && !equal;i++) equal |= op[i] == num.op[i];
		} else {
			for(uint16_t i=op_size;i<num.op_size && !equal;i++) equal |= num.op[i] == 0;
			for(uint16_t i=0;i<op_size && !equal;i++) equal |= op[i] == num.op[i];
		}
		return equal;
	}
	
	// check if initialized and not zero
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint boolean not operator!")]]
	constexpr bool BigUint<bitsize>::operator!()
	{
		bool notzero = 0;
		for(uint16_t i=0;i<op_size;i++) {
			if(op[i] != 0) {
				notzero = 1;
				break;
			}
		}
		return notzero;
	}

	// TODO: make sure that all of the following comparision operators work with different op-sizes, they are currently not designed to.
	
	// boolean operator, check if not equal to
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint boolean not equal to operator!=")]]
	constexpr bool BigUint<bitsize>::operator!=(const BigUint &num)
	{
		bool notequal = 0;
		if(op_size >= num.op_size) {
			for(uint16_t i=num.op_size;i<op_size && !notequal;i++) notequal |= op[i] != 0;
			for(uint16_t i=0;i<num.op_size && !notequal;i++) notequal |= op[i] != num.op[i];
		} else {
			for(uint16_t i=op_size;i<num.op_size && !notequal;i++) notequal |= num.op[i] != 0;
			for(uint16_t i=0;i<op_size && !notequal;i++) notequal |= op[i] != num.op[i];
		}
		return notequal;
	}
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint boolean less than operator<")]]
	constexpr bool BigUint<bitsize>::operator<(const BigUint &num)
	{
		bool less = 0;
	
		// condition to avoid iterating over non-existing members of op
		const uint16_t iterator = op_size < num.op_size ? op_size : num.op_size;
		for(uint16_t i=0;i<iterator;i++) {
			if(op[i] < num.op[i]) {
				less = 1;
				break;
			} else {
				if(op[i] == num.op[i]) continue;
				break;
			}
		}
		return less;
	}
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint less or equal to operator<=")]]
	constexpr bool BigUint<bitsize>::operator<=(const BigUint &num)
	{
		bool less = 0; // or equal
	
		// condition to avoid iterating over non-existing members of op
		const uint16_t iterator = op_size < num.op_size ? op_size : num.op_size;
		for(uint16_t i=0;i<iterator;i++) {
			if(op[i] <= num.op[i]) {
				less = 1;
				break;
			} else {
				break;
			}
		}
		return less;
	}
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint greater operator>")]]
	constexpr bool BigUint<bitsize>::operator>(const BigUint &num)
	{
		bool greater = 0; // or equal
	
		// condition to avoid iterating over non-existing members of op
		const uint16_t iterator = op_size < num.op_size ? op_size : num.op_size;
		for(uint16_t i=0;i<iterator;i++) {
			if(op[i] > num.op[i]) {
				greater = 1;
				break;
			} else {
				if(op[i] == num.op[i]) continue;
				break;
			}
		}
		return greater;
	}
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint greater or equal to operator>=")]]
	constexpr bool BigUint<bitsize>::operator>=(const BigUint &num)
	{
		bool greater = 0; // or equal
	
		// condition to avoid iterating over non-existing members of op
		const uint16_t iterator = op_size < num.op_size ? op_size : num.op_size; // TODO: possibly check if bigger op_size breaks to comparision
		for(uint16_t i=0;i<iterator;i++) {
			if(op[i] >= num.op[i]) {
				greater = 1;
				break;
			} else {
				break;
			}
		}
		return greater;
	}
	
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator+")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator+(const BigUint &num)
	{
		uint64_t new_op[op_size];
		std::copy(std::begin(op), std::end(op), std::begin(new_op)); // set to op
		std::cout << std::hex << std::endl << "old op:\t";
		for(uint16_t i=0;i<op_size;i++) {
			std::cout << new_op[i] << " ";
		}
		for(uint16_t i=op_size;i --> 0;) {
			__uint128_t tmp = op[i];
			tmp += num.op[i];
			if(tmp > UINT64_MAX) {
	    		new_op[i] = tmp & 64; // assign the main value to assign value with no carry (only no carry because of bit-shifting)
				uint16_t j = 1;
				while(true) { // if carry exists
					__uint128_t new_tmp = (tmp >> 64)+new_op[i+j];
					if(new_tmp > UINT64_MAX) {
	    				new_op[i+j] = new_tmp & UINT64_MAX;
					} else {
						new_op[i+j] = new_tmp;
						break;
					}
					j++;
				}
			} else {
				new_op[i] = tmp;
			}
		}
	
		// testing
		std::cout << std::endl << "new_op:\t";
		for(uint16_t i=0;i<op_size;i++) {
			std::cout << new_op[i] << "";
		}
		std::cout << std::endl << "answer: " << "466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
	
		std::cout << std::endl << "returned" << std::endl;
		return BigUint<bitsize>(new_op, op_size);
	}
	
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpragmas"
	#pragma GCC diagnostic ignored "-Wc++2b-extensions" // for some reason raises -Wpragmas because warning not found while it surpresses the warning at the same time
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator-")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator-(const BigUint &num)
	{
		// 0x7fffffffffffffffffffffffffffffffU // largest signed 128-bit num
	    static constexpr const __int128_t int128_max = ((__int128_t)0x7fffffffffffffffU<<64)|0xffffffffffffffffU;
		
		// for efficiency, check each index of op. If any of the indexes are the same, cancel it. Else subtract. Don't compare the whole biginteger at once
		if(op_size >= num.op_size) { // result is 0 or bigger
			// do reverse. Like on paper
			for(uint16_t i=num.op_size;i --> 0;) {
				
			}
		} else { // result requires extra unsigned arithmetic to keep it bigger than 0
			for(uint16_t i=op_size;i --> 0;) {
				
			}
			
		}
		return *this;
	}
	#pragma GCC diagnostic pop
};

#endif /* BIGINT_CPP */
