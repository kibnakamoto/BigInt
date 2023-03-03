#ifndef BIGINT_CPP

#include <iostream>
#include <string>
#include <stdint.h>

#include "bigint.h"

template<uint16_t bitsize>
BigInt<bitsize>::BigInt(std::string input)
{
	strtobigint(input);
}

template<uint16_t bitsize>
BigInt<bitsize>::BigInt(const char* input)
{
	strtobigint(std::string(input)); // to avoid annoying C++ conversion error
}

// numerical input. If number is 256-bit, input = left 128-bit, right 128-bit
template<uint16_t bitsize> 
template<size_t count> // number of arguements
BigInt<bitsize>::BigInt(__uint128_t input, ...) {
    // uint128_t input to 2 uint64_t integers
	// constant mask values
    static constexpr const __uint128_t bottom_mask = (__uint128_t{1} << 64) - 1; // 0x0000000000000000ffffffffffffffffU
    static constexpr const __uint128_t top_mask = ~bottom_mask;                  // 0xffffffffffffffff0000000000000000U

	// pad the operator array
	op_nonleading_i = op_size-(count<<1)-1;
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
//		for(uint16_t i=num.op_size;i>=0;i++) op[i] = num.op[i];
//		for(uint16_t i=num.op_size;i<op_size;i++) op[i] = num.op[i];
//	}
//	return *this;
//}
//#pragma GCC diagnostic pop

template<uint16_t bitsize>
BigInt<bitsize> BigInt<bitsize>::operator=(const std::string &num)
{
	delete this; // object suicide
	return BigInt<bitsize>(num); // reconstruct as new object
}

int main()
{
	uint256_t num="256";
	uint256_t num2 = std::string("256");
	// constexpr uint256_t num3 = BigInt<256><1>(256);
	std::cout << "num:" << num;
	std::cout << "\nnum2: " << num2;
	// std::cout << "\nnum3: " << num3;
	return 0;
}

#endif /* BIGINT_CPP */
