#ifndef BIGINT_CPP
#define BIGINT_CPP

#include <iostream>
#include <string>
#include <stdint.h>
#include <cstdarg>
#include <array>
#include <utility>

#include "bigint.h"

// NOTE: all operators work for the same op size. Maybe remove the conditions that define it otherwise

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
	
	// numerical input constructor. If number is 256-bit, input = left 128-bit, right 128-bit
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpragmas"
	#pragma GCC diagnostic ignored "-Wc++2b-extensions"
	#pragma GCC diagnostic ignored "-Wc++17-extensions"
	#pragma GCC diagnostic ignored "-Wvarargs"
	template<uint16_t bitsize> 
	constexpr BigUint<bitsize>::BigUint(const uint16_t count, __uint128_t ...) {
		std::va_list args;
		va_start(args, count);
	
		// pad the operator array
		const uint16_t count64 = count << 1; // count if input is 64-bits
		op_nonleading_i = count64 < op_size? op_size-count64 : 0;
		for(uint16_t i=0;i<=op_nonleading_i;i++) op[i] = 0x0000000000000000ULL;
	
		// add the inputs to the operator array
	    for(size_t i=0;i<count;i++) {
	        __uint128_t num = va_arg(args, __uint128_t);
	        op[op_size-i*2-1] = num&top_mask_u128; ////////////////////// RECENT CHANGE - maybe change indexes if problem
	        op[op_size-i*2-2] = num&bottom_mask_u128;
	    }
		va_end(args);
	}

	// numerical input. If number is 256-bit, input = left 128-bit, right 128-bit, same as the function above except it's compile-time
	template<uint16_t bitsize>
	template<typename ...Ts> // all uint128_t, must specify
	consteval BigUint<bitsize> BigUint<bitsize>::assign_conste(Ts... input) noexcept
	{
		// pad the operator array
		constexpr const size_t count = sizeof...(Ts);
		const constexpr uint16_t count64 = count << 1; // count if input is 64-bits
		if constexpr(count64 < op_size)
				op_nonleading_i = op_size-count64;
		else
			op_nonleading_i = 0;
		for(uint16_t i=0;i<=op_nonleading_i;i++) op[i] = 0x0000000000000000ULL;
	
		// add the inputs to the operator array
		uint16_t i=0;
	    for(const auto num : {input...}) {
	        op[op_size-i*2-1] = num&top_mask_u128; ////////////////////// RECENT CHANGE - maybe change indexes if problem
	        op[op_size-i*2-2] = num&bottom_mask_u128;
			i++;
	    }
		return *this;
	}
	#pragma GCC diagnostic pop

	// assignment to operator array of known length
	template<uint16_t bitsize>
	constexpr BigUint<bitsize>::BigUint(uint64_t *input, uint16_t len) // input order has to be: input[0] = most left 64-bit
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

	// helper function to assign compile time array so that it can be assigned to op as compile time
	// Basically the above constructor as a constexpr function
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wc++20-extensions" // not initialized on decleration on constexpr function
	#pragma GCC diagnostic ignored "-Wc++17-extensions"
	template<uint16_t bitsize>
	template<uint16_t len, std::array<uint64_t, len> input>
	consteval BigUint<bitsize> BigUint<bitsize>::assign_op() noexcept
	{
		// calculate pad count
		if(len > op_size) { // if bigger integer type, and input array has non-zero number at the height of op
			bool found = false;

			for(uint16_t i=0;i<=op_size;i++) { // iterate to op_size because input array order is backwards
				if constexpr(input[i] != 0)
					found = true;
			}
			if (found) { // maybe input[op_size] = 0 but input[op_size+1] !=0, therefore use the found defined above
				throw int_too_large_error("given integer is too large for the defined BigUint");
			} else {
				uint16_t nonzeroi; // index of non-zero input element
				for(uint16_t i=op_size+1;i<len;i++) { // basically continuation of previous loop
					if constexpr(input[i] != 0) {
						nonzeroi=i;
						break;
					}
				}
	    		// pad the operator array
	    		for(uint16_t i=0;i<nonzeroi;i++) op[i] = 0x0000000000000000ULL;
				op_nonleading_i = nonzeroi;

	    		// add input to operator array
	    		for(uint16_t i=op_size;i --> nonzeroi;) op[i] = input[i];
			}
		} else {
			const uint16_t constexpr pad_count = len-op_size;

	    	// pad the operator array
	    	for(uint16_t i=0;i<pad_count;i++) op[i] = 0x0000000000000000ULL;
	
	    	// add input to operator array
	    	for(uint16_t i=op_size;i --> pad_count;) op[i] = input[i];
		}
		return *this;
	}
	#pragma GCC diagnostic pop
	
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
	
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wc++17-extensions"
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint boolean equal to operator==")]]
	constexpr bool BigUint<bitsize>::operator==(const BigUint &num)
	{
		bool equal = 0;
		if constexpr(op_size >= num.op_size) {
			for(uint16_t i=num.op_size;i<op_size && !equal;i++) equal |= op[i] == 0;
			for(uint16_t i=0;i<num.op_size && !equal;i++) equal |= op[i] == num.op[i];
		} else {
			for(uint16_t i=op_size;i<num.op_size && !equal;i++) equal |= num.op[i] == 0;
			for(uint16_t i=0;i<op_size && !equal;i++) equal |= op[i] == num.op[i];
		}
		return equal;
	}
	#pragma GCC diagnostic pop
	
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

	// boolean operator, check if not equal to
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wc++17-extensions"
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint boolean not equal to operator!=")]]
	constexpr bool BigUint<bitsize>::operator!=(const BigUint &num)
	{
		bool notequal = 0;
		if constexpr(op_size >= num.op_size) {
			for(uint16_t i=num.op_size;i<op_size && !notequal;i++) notequal |= op[i] != 0;
			for(uint16_t i=0;i<num.op_size && !notequal;i++) notequal |= op[i] != num.op[i];
		} else {
			for(uint16_t i=op_size;i<num.op_size && !notequal;i++) notequal |= num.op[i] != 0;
			for(uint16_t i=0;i<op_size && !notequal;i++) notequal |= op[i] != num.op[i];
		}
		return notequal;
	}
	#pragma GCC diagnostic pop

	// TODO: make sure that all of the following comparision operators work with different op-sizes, they are currently not designed to.
	
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint boolean less than operator<")]]
	constexpr bool BigUint<bitsize>::operator<(const BigUint &num)
	{
		bool less = 0;
	
		// condition to avoid iterating over non-existing members of op
		const constexpr uint16_t iterator = op_size < num.op_size ? op_size : num.op_size;
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
		const uint16_t constexpr iterator = op_size < num.op_size ? op_size : num.op_size;
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
		const constexpr uint16_t iterator = op_size < num.op_size ? op_size : num.op_size;
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
		const constexpr uint16_t iterator = op_size < num.op_size ? op_size : num.op_size;
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
		// std::cout << std::endl << "new_op:\t";
		// for(uint16_t i=0;i<op_size;i++) {
		// 	std::cout << new_op[i] << "";
		// }
		// std::cout << std::endl << "answer: " << "466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2";
	
		// std::cout << std::endl << "returned" << std::endl;
		return BigUint<bitsize>(new_op, op_size);
	}

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpragmas"
	#pragma GCC diagnostic ignored "-Wc++2b-extensions" // for some reason raises -Wpragmas because warning not found while it surpresses the warning at the same time
	#pragma GCC diagnostic ignored "-Wc++17-extensions"
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator-")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator-(const BigUint &num)
	{
		if constexpr(op_size >= num.op_size) { // result is bigger than num, default condition, if removed
			uint64_t ret[op_size]; // make sure to assign the biggest data size in the subtraction so that there is less chance of a value being sent to the void
			for(uint16_t i=0;i<num.op_size;i++) { // cover 0 to num.op_size (smaller)
				if (op[i] < num.op[i]) {
					ret[i] = (UINT64_MAX - num.op[i]) + op[i];
				} else {
					ret[i] = op[i] - num.op[i];
				}
			}
			if constexpr(op_size != num.op_size) {
				for(uint64_t i=num.op_size;i<op_size;i++) {
					ret[i] = op[i];
				}
			}
			return BigUint<bitsize>(ret, op_size);
		} else {
			uint64_t ret[num.op_size];
			for(uint16_t i=0;i<op_size;i++) {
				if (op[i] < num.op[i]) {
					ret[i] = (UINT64_MAX - num.op[i]) + op[i];
				} else {
					ret[i] = op[i] - num.op[i];
				}
				
			}
			for(uint64_t i=op_size;i<num.op_size;i++) {
				ret[i] = UINT64_MAX-num.op[i];
			}
			
			return BigUint<bitsize>(ret, num.op_size);
		}
	}
	#pragma GCC diagnostic pop

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator*")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator*(const BigUint &num)
	{
		uint64_t ret[op_size];
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator++")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator++()
	{
		for(uint16_t i=op_size;i --> 0;) {
			if(op[i] != UINT64_MAX) op[i]++;
			else {
				if(i != 0) continue;
				else {
					for(uint16_t j=0;j<op_size;j++) op[j] = 0x0000000000000000ULL;
					break;
				}
			}
		}
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator--")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator--()
	{
		for(uint16_t i=0;i<op_size;i++) {
			if(op[i] != 0) op[i]--;
			else {
				if(i != op_size-1) continue;
				else {
					for(uint16_t j=0;j<op_size;j++) op[j] = 0xffffffffffffffffULL;
					break;
				}
			}
		}
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator[] for accessing bit")]]
	constexpr bool BigUint<bitsize>::operator[](const uint32_t &index) const
	{
		const uint64_t mod = index%64;
		return op[!mod ? index/64 : index/64+1] & (1 << mod);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator[] for accessing 64-bits")]]
	constexpr uint64_t BigUint<bitsize>::operator[](const uint16_t &index) const
	{
		return op[index];
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator~")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator~() const
	{
		uint64_t ret[op_size];
		for(uint16_t i=0;i<op_size;i++)  ret[i] = ~op[i];
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator&")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator&(const BigUint &num)
	{
		uint64_t ret[op_size];
		for(uint16_t i=0;i<op_size;i++)  ret[i] = op[i] & num.op[i];
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator&=")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator&=(const BigUint &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		for(uint16_t i=0;i<op_size;i++)  op[i] &= num.op[i];
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator^")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator^(const BigUint &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		uint64_t ret[op_size];
		for(uint16_t i=0;i<op_size;i++)  ret[i] = op[i] ^ num.op[i];
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator^=")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator^=(const BigUint &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		for(uint16_t i=0;i<op_size;i++)  op[i] ^= num.op[i];
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator>>")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator>>(BigUint num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		uint64_t ret[op_size];
		for(uint16_t i=0;i<op_size;i++) {
			if(num < 64) {
				ret[i] = op[i] >> num;
				break;
			} else {
				ret[i] = 0;
				num-=64;
			}
		}
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator>>=")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator>>=(BigUint num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		for(uint16_t i=0;i<op_size;i++) {
			if(num < 64) {
				op[i] >>= num;
				break;
			} else {
				op[i] = 0;
				num-=64;
			}
		}
		// assuming they are the same size. Which should be enforced by compiler by default
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator<<")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator<<(BigUint num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		uint64_t ret[op_size];
		for(uint16_t i=op_size;i --> 0;) {
			if(num < 64) {
				ret[i] = op[i] << num;
				break;
			} else {
				ret[i] = UINT64_MAX;
				num-=64;
			}
		}
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator<<=")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator<<=(BigUint num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		for(uint16_t i=op_size;i --> 0;) {
			if(num < 64) {
				op[i] <<= num;
				break;
			} else {
				op[i] = UINT64_MAX;
				num-=64;
			}
		}
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator>>")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator>>(const uint64_t &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		uint64_t ret[op_size];
		ret[0] = op[0] >> num;
		for(uint16_t i=1;i<op_size;i++) ret[i] = op[i];
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator>>=")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator>>=(const uint64_t &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		op[0] >>= num;
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator<<")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator<<(const uint64_t &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		uint64_t ret[op_size];
		const constexpr uint16_t index = op-size-1;
		ret[index] = op[index] >> num;
		for(uint16_t i=index;i --> 0;) ret[i] = op[i];
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator<<=")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator<<=(const uint64_t &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		op[op-size-1] <<= num;
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator|")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator|(const BigUint &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		uint64_t ret[op_size];
		for(uint16_t i=0;i<op_size;i++)  ret[i] = op[i] | num.op[i];
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator|=")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator|=(const BigUint &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		for(uint16_t i=0;i<op_size;i++)  op[i] |= num.op[i];
		return *this;
	}
}; /* NAMESPACE BIGINT */

#endif /* BIGINT_CPP */
