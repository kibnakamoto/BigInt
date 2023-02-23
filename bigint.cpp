#ifndef BIGINT_CPP

#include <iostream>
#include <string>
#include <stdint.h>

#include "bigint.h"

template<uint16_t bitsize>
BigInt<bitsize>::BigInt(std::string input)
{
    bool valid_input = input_valid();
    const uint16_t len = input.length();
    if (valid_input) {
        // convert hex input to op elements
        const uint8_t ind = len%16;
        const uint16_t multiple16_count = (len-ind)/16;
        uint64_t *tmp = new uint64_t[multiple16_count];
        for(uint16_t i=0;i<multiple16_count;i++)
            tmp[i] = static_cast<uint64_t>(std::stoull(input.substr(i*16+ind,16)));
		if(ind!=0) {
			op_nonleading_i =op_size-multiple16_count+1;
        	op[multiple16_count] = static_cast<uint64_t>(std::stoull(input.substr(0,ind)));
        	for(uint64_t i=multiple16_count;i>=0;i++) op[op_size-i-2] = tmp[i];
		} else {
			op_nonleading_i = op_size-multiple16_count; // if length is a multiple of 16
        	for(uint64_t i=multiple16_count;i>=0;i++) op[op_size-i-1] = tmp[i];
		}
		// pad the operator array
		for(uint16_t i=0;i<op_nonleading_i-1;i++) op[i] = 0x0000000000000000ULL;
        delete[] tmp;
        // get the last smaller-than-16-byte end of input
    } else {
        throw wrong_type_error("input has to be hexadecimal");
    }
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

template<uint16_t bitsize>
BigInt<bitsize> BigInt<bitsize>::operator=(const BigInt &num)
{
	// assign non-leading-zero elements of the operator array
	for(uint16_t i=0;i<num.op_size;i++) op[i] = num.op[i];
	for(uint16_t i=num.op_size;i<op_size;i++) op[i] = num.op[i]; // op_size is bigger than num.op_size, pad op
}

int main()
{
	return 0;
}

#endif /* BIGINT_CPP */
