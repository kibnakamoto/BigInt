#ifndef BIGINT_CPP

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
        op[op_size-i-1] = num&bottom_mask;
        op[op_size-i-2] = (num&top_mask) >> 64;
    }
}

template<uint16_t bitsize>
BigInt<bitsize>::BigInt(uint64_t *input, uint16_t len) // input order has to be: input[0] = most left 64-bit
{
    const uint16_t pad_count = op_size-len;

    // pad the operator array
    for(uint16_t i=0;i<pad_count;i++) op[i] = 0x0000000000000000ULL;

    // add input to operator array
    for(uint16_t i=op_size-1;i>=pad_count;i--) op[i] = input[op_size-i-1];
	op_nonleading_i = pad_count;
}

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
[[nodiscard("discarded assignment operator")]]
constexpr BigInt<bitsize> BigInt<bitsize>::operator=(const char* &num)
{
	delete this; // object suicide
	return BigInt<bitsize>(num); // reconstruct as new object
}

template<uint16_t bitsize>
[[nodiscard("discarded BigInt operator+")]]
constexpr BigInt<bitsize> BigInt<bitsize>::operator+(const BigInt &num)
{
	uint64_t new_op[op_size];
	for(uint16_t i=0;i<op_size;i++) {
		__uint128_t tmp = op[i];
		tmp += num[i];
		if(tmp > UINT64_MAX) { // TODO: debug and test
    		new_op[i] = tmp << 64;
    		new_op[i+1] = tmp & UINT64_MAX;
		} else {
			new_op[i] = tmp;
		}
	}
	return BigInt<bitsize>(new_op, op_size);
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
	// std::cout << "\nnum3: " << num3;
	std::cout << std::endl;
	return 0;
}

#endif /* BIGINT_CPP */
