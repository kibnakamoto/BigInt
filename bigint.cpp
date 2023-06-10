#ifndef BIGINT_CPP
#define BIGINT_CPP

#include <iostream>
#include <string>
#include <stdint.h>
#include <cstdarg>
#include <array>
#include <utility>
#include <bitset>
#include <sstream>

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
	        op[op_size-i*2-1] = num >> 64; ////////////////////// RECENT CHANGE - maybe change indexes if problem
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
	        op[op_size-i*2-1] = num >> 64; ////////////////////// RECENT CHANGE - maybe change indexes if problem
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
		uint64_t tmp_op[op_size];
		std::copy(std::begin(op), std::end(op), std::begin(tmp_op)); // set to op
		for(uint16_t i=op_size;i --> 0;) {
			new_op[i] = 0;
			__uint128_t tmp = tmp_op[i];
			tmp += num.op[i];
			if(tmp > UINT64_MAX) {
	    		new_op[i] += tmp & UINT64_MAX; // assign the main value to assign value with no carry (only no carry because of bit-shifting)
				uint16_t j = 1;
				while(new_op[i-j] == UINT64_MAX and i+j < op_size-1) {
					j++;
				}
	    		tmp_op[i-j]++;
			} else {
				new_op[i] += tmp;
			}
		}
	
		return BigUint<bitsize>(new_op, op_size);
	}

	template<uint16_t bitsize>
	constexpr BigUint<bitsize> BigUint<bitsize>::operator+=(const BigUint &num)
	{
		uint64_t tmp_op[op_size];
		std::copy(std::begin(op), std::end(op), std::begin(tmp_op)); // set to op
		for(uint16_t i=op_size;i --> 0;) {
			op[i] = 0;
			__uint128_t tmp = tmp_op[i];
			tmp += num.op[i];
			if(tmp > UINT64_MAX) {
	    		op[i] = tmp & UINT64_MAX; // assign the main value to assign value with no carry (only no carry because of bit-shifting)
				uint16_t j = 1;
				while(op[i-j] == UINT64_MAX and i+j < op_size-1) { // carry index
					j++;
				}
	    		tmp_op[i-j]++; // carry
			} else {
				op[i] += tmp;
			}
		}
		return *this;
	}

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpragmas"
	#pragma GCC diagnostic ignored "-Wc++2b-extensions" // for some reason raises -Wpragmas because warning not found while it surpresses the warning at the same time
	#pragma GCC diagnostic ignored "-Wc++17-extensions"
	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator-")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator-(const BigUint &num)
	{
		uint64_t ret[op_size];
		uint64_t new_op[op_size];
		std::copy(std::begin(op), std::end(op), std::begin(new_op)); // set to op
		for(uint16_t i=op_size;i --> 0;) {
			ret[i] = 0;
			if (new_op[i] < num.op[i]) {
				ret[i] = new_op[i]-num.op[i];
				uint16_t j=0;
				while(ret[i-1-j] == 0) j++;
				new_op[i-1-j]--;
			} else {
				ret[i] = new_op[i] - num.op[i];
			}
		}
		return BigUint<bitsize>(ret, op_size);
	}
	#pragma GCC diagnostic pop

	template<uint16_t bitsize>
	constexpr BigUint<bitsize> BigUint<bitsize>::operator-=(const BigUint &num)
	{
		for(uint16_t i=op_size;i --> 0;) {
			if (op[i] < num.op[i]) {
				op[i] -= num.op[i];
				op[i-1]--;
			} else {
				op[i] -= num.op[i];
			}
		}
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator*")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator*(BigUint num)
	{
		// Russian Peasant Algorithm
		uint64_t o[op_size];
		for(uint16_t i=0;i<op_size;i++) o[i] = op[i];
		BigUint<bitsize> new_op = BigUint<bitsize>(o, op_size);
		BigUint<bitsize> ret = 0;
		while(num > "0") {
			if(num & "1") ret += new_op;
			new_op <<= 1;
			num >>= 1;
		}
		return ret;
	}

	template<uint16_t bitsize>
	constexpr BigUint<bitsize> BigUint<bitsize>::operator*=(BigUint num)
	{
		*this = *this * num;
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator/")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator/(const BigUint &num)
	{

	    uint16_t bits_left = op_size*4;
		auto quot = *this;
		BigUint<bitsize> rem = 0;
		do {
			BigUint<bitsize> t = quot;
			quot *= 2;
			rem += quot < t;
			if (rem >= num) {
				rem-=num;
				quot+=1;
			}
			bits_left--;
		} while(bits_left);
		return quot;
	
	    //uint32_t quot, rem, t;
	    //quot = dividend;
	    //rem = 0;
	    //do {
	    //        // (rem:quot) << 1
	    //        t = quot;
	    //        quot = quot + quot;
	    //        rem = rem + rem + (quot < t);
	
	    //        if (rem >= divisor) {
	    //            rem = rem - divisor;
	    //            quot = quot + 1;
	    //        }
	    //        bits_left--;
	    //} while (bits_left);
	    //return quot;
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
	constexpr BigUint<bitsize> BigUint<bitsize>::operator^=(const BigUint &num)
	{
		// assuming they are the same size. Which should be enforced by compiler by default
		for(uint16_t i=0;i<op_size;i++)  op[i] ^= num.op[i];
		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator>>")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator>>(const uint16_t &num)
	{
		uint64_t ret[op_size];
		if(num >= bitsize) {
			for(uint16_t i=0;i<op_size;i++) ret[i] = 0;
			return BigUint<bitsize>(ret, op_size);
		}

		std::stringstream buf;
		for(uint16_t i=0;i<op_size;i++) {
			std::bitset<64> tmp(op[i]);
			buf << tmp.to_string();
		}
		std::bitset<bitsize> bits(buf.str());
		bits >>= num;
		buf.clear();
		std::string str = bits.to_string();
		std::string out = "";
		for(uint16_t i=0;i<op_size;i++) {
			std::bitset<64> buffer(str.substr(i*64, i*64+64));
			ret[i] = buffer.to_ullong();
		}

		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	constexpr BigUint<bitsize> BigUint<bitsize>::operator>>=(const uint16_t &num)
	{
		if(num >= bitsize) {
			for(uint16_t i=0;i<op_size;i++) op[i] = 0;
			return *this;
		}
		std::stringstream buf;
		for(uint16_t i=0;i<op_size;i++) {
			std::bitset<64> tmp(op[i]);
			buf << tmp.to_string();
		}
		std::bitset<bitsize> bits(buf.str());
		bits >>= num;
		buf.clear();
		std::string str = bits.to_string();
		std::string out = "";
		for(uint16_t i=0;i<op_size;i++) {
			std::bitset<64> buffer(str.substr(i*64, i*64+64));
			op[i] = buffer.to_ullong();
		}

		return *this;
	}

	template<uint16_t bitsize>
	[[nodiscard("discarded BigUint operator<<")]]
	constexpr BigUint<bitsize> BigUint<bitsize>::operator<<(const uint16_t &num)
	{
		uint64_t ret[op_size];
		if(num >= bitsize) {
			for(uint16_t i=0;i<op_size;i++) ret[i] = 0;
			return BigUint<bitsize>(ret, op_size);
		}

		std::stringstream buf;
		for(uint16_t i=0;i<op_size;i++) {
			std::bitset<64> tmp(op[i]);
			buf << tmp.to_string();
		}
		std::bitset<bitsize> bits(buf.str());
		bits <<= num;
		buf.clear();
		std::string str = bits.to_string();
		std::string out = "";
		for(uint16_t i=0;i<op_size;i++) {
			std::bitset<64> buffer(str.substr(i*64, i*64+64));
			ret[i] = buffer.to_ullong();
		}
		return BigUint<bitsize>(ret, op_size);
	}

	template<uint16_t bitsize>
	constexpr BigUint<bitsize> BigUint<bitsize>::operator<<=(const uint16_t &num)
	{
		if(num >= bitsize) {
			for(uint16_t i=0;i<op_size;i++) op[i] = 0;
			return *this;
		}
		std::stringstream buf;
		for(uint16_t i=0;i<op_size;i++) {
			std::bitset<64> tmp(op[i]);
			buf << tmp.to_string();
		}
		std::bitset<bitsize> bits(buf.str());
		bits <<= num;
		buf.clear();
		std::string str = bits.to_string();
		std::string out = "";
		for(uint16_t i=0;i<op_size;i++) {
			std::bitset<64> buffer(str.substr(i*64, i*64+64));
			op[i] = buffer.to_ullong();
		}

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
