#ifndef BIGINT_CPP
#define BIGINT_CPP

#include <iostream>
#include <string>
#include <stdint.h>

#include "bigint.h"

template<uint16_t bitsize>
template<uint8_t base> // type of input (oct = base 8, hex = base 16)
BigInt<bitsize>::BigInt(std::string input)
{
	strtobigint<base>(input.c_str());
}

template<uint16_t bitsize>
template<uint8_t base> // type of input (oct = base 8, hex = base 16)
BigInt<bitsize>::BigInt(const char* input)
{
	strtobigint<base>(input); // to avoid annoying C++ conversion error
}

// numerical input. If number is 256-bit, input = left 128-bit, right 128-bit
template<uint16_t bitsize> 
template<size_t count> // number of arguments
BigInt<bitsize>::BigInt(__uint128_t input, ...) {
    // uint128_t input to 2 uint64_t integers
	// constant mask values
    static constexpr const __uint128_t bottom_mask = (__uint128_t{1} << 64) - 1; // 0x0000000000000000ffffffffffffffffU
    static constexpr const __uint128_t top_mask = ~bottom_mask;                  // 0xffffffffffffffff0000000000000000U

	// pad the operator array
	const constexpr uint16_t tmp = op_size-(count<<1)-1;
	op_nonleading_i = tmp;
	for(uint16_t i=0;i<op_nonleading_i;i++) op[i] = 0x0000000000000000ULL;

	// add the inputs to the operator array
    for(size_t i=0;i<count;i++) {
        __uint128_t num = *((__uint128_t*)&input + i);
        op[op_size-i*2-1] = num&bottom_mask;
        op[op_size-i*2-2] = (num&top_mask) >> 64;
    }
}

// assignment to operator array of known length
template<uint16_t bitsize>
BigInt<bitsize>::BigInt(uint64_t *input, uint16_t len) // input order has to be: input[0] = most left 64-bit
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
			throw int_too_large_error(("given integer is too large for the defined BigInt<" + std::to_string(bitsize) +
									  "> which is not enough to hold a value of BigInt<" + std::to_string(len*64) + ">").c_str());
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

//#pragma GCC diagnostic ignored "-Wtype-limits"
//template<uint16_t bitsize>
//BigInt<bitsize> BigInt<bitsize>::operator=(const BigInt &num)
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
[[nodiscard("discarded BigInt assignment operator")]]
constexpr BigInt<bitsize> BigInt<bitsize>::operator=(const char* &num)
{
	delete this; // object suicide
	return BigInt<bitsize>(num); // reconstruct as new object
}

template<uint16_t bitsize>
[[nodiscard("discarded BigInt boolean and operator&&")]]
constexpr bool BigInt<bitsize>::operator&&(const BigInt &num)
{
	
	return 0;
}

template<uint16_t bitsize>
[[nodiscard("discarded BigInt boolean or operator||")]]
constexpr bool BigInt<bitsize>::operator||(const BigInt &num)
{
	return 0;
}

template<uint16_t bitsize>
[[nodiscard("discarded BigInt boolean equal to operator==")]]
constexpr bool BigInt<bitsize>::operator==(const BigInt &num)
{
	return 0;
}

// check if initialized and not zero
template<uint16_t bitsize>
[[nodiscard("discarded BigInt boolean not operator!")]]
constexpr bool BigInt<bitsize>::operator!()
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
template<uint16_t bitsize>
[[nodiscard("discarded BigInt boolean not equal to operator!=")]]
constexpr bool BigInt<bitsize>::operator!=(const BigInt &num)
{
	// the smaller iterator, smallest op_size, use to iterate safely without getting signal SEGV
	bool notequal = 0;
	if(op_size >= num.op_size) {
		const uint16_t smaller_iter = num.op_size;
		const uint16_t bigger_iter = op_size;
		for(uint16_t i=smaller_iter;i<bigger_iter && !notequal;i++) notequal |= op[i] != 0;
		for(uint16_t i=0;i<smaller_iter && !notequal;i++) notequal |= op[i] != num.op[i];
	} else {
		const uint16_t smaller_iter = op_size;
		const uint16_t bigger_iter = num.op_size;
		for(uint16_t i=smaller_iter;i<bigger_iter && !notequal;i++) notequal |= num.op[i] != 0;
		for(uint16_t i=0;i<smaller_iter && !notequal;i++) notequal |= op[i] != num.op[i];
	}
	return notequal;
}

template<uint16_t bitsize>
[[nodiscard("discarded BigInt boolean less than operator<")]]
constexpr bool BigInt<bitsize>::operator<(const BigInt &num)
{
	return 0;
}

template<uint16_t bitsize>
[[nodiscard("discarded BigInt less or equal to operator<=")]]
constexpr bool BigInt<bitsize>::operator<=(const BigInt &num)
{
	return 0;
}

template<uint16_t bitsize>
[[nodiscard("discarded BigInt greater operator>")]]
constexpr bool BigInt<bitsize>::operator>(const BigInt &num)
{
	return 0;
}

template<uint16_t bitsize>
[[nodiscard("discarded BigInt greater or equal to operator>=")]]
constexpr bool BigInt<bitsize>::operator>=(const BigInt &num)
{
	return 0;
}


template<uint16_t bitsize>
[[nodiscard("discarded BigInt operator+")]]
constexpr BigInt<bitsize> BigInt<bitsize>::operator+(const BigInt &num)
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
	return BigInt<bitsize>(new_op, op_size);
}

template<uint16_t bitsize>
[[nodiscard("discarded BigInt operator+")]]
constexpr BigInt<bitsize> BigInt<bitsize>::operator-(const BigInt &num)
{
	// 0x7fffffffffffffffffffffffffffffffU // largest signed 128-bit num
    static constexpr const __int128_t int128_max = ((__int128_t)0x7fffffffffffffffU<<64)|0xffffffffffffffffU;
	
	// first check if number is bigger
}

int main()
{
	// uint256_t num="256";
	// uint256_t num2 = std::string("256");
	uint256_t num="2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
	uint256_t num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
	// constexpr uint256_t num3 = BigInt<256><1>(256);
	std::cout << "int num: " << num;
	std::cout << "\nhex num2: " << std::hex << num2;
	auto num3 = num + num2;
	std::cout << "\nnum+num2: " << num3;
	std::cout << std::endl;
	return 0;
}

#endif /* BIGINT_CPP */
