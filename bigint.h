#ifndef BIGINT_H
#define BIGINT_H

#include <cstdint>
#include <string>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <exception>
#include <array>
#include <type_traits>
#include <iostream>

namespace BigInt
{
	
	// raise when wrong type
	class wrong_type_error : public std::runtime_error {
		public: explicit wrong_type_error(const char *str) : std::runtime_error(str) {}
	};
	
	// raise when integer too large
	class int_too_large_error : public std::runtime_error {
		public: explicit int_too_large_error(const char *str) : std::runtime_error(str) {}
	};
	
	// custom-size integer class
	template<uint16_t bitsize>
	// alignas((bitsize%64==0 ? bitsize/64 : bitsize/64+1)<<3)
	class BigUint {
		protected:
			// operator array
			const constexpr static uint16_t op_size = bitsize%64==0 ? bitsize/64 : bitsize/64+1;
			uint64_t op[op_size]; // when iterating, start from end to start
			uint16_t op_nonleading_i; // index of op when leading zeros end

			// uint128_t input to 2 uint64_t integers
			// constant mask values
		    static constexpr const __uint128_t bottom_mask_u128 = (__uint128_t{1} << 64) - 1; // 0x0000000000000000ffffffffffffffffU
	    	static constexpr const __uint128_t top_mask_u128 = ~bottom_mask_u128;                  // 0xffffffffffffffff0000000000000000U

			// get substring of char* - helper function
			constexpr std::string get_substring(const char* str, uint16_t start, uint16_t substrsize)
			{
				std::string substr = "";
			
				// Copy the characters from str[startIndex] to str[endIndex]
				const uint16_t width = substrsize+start;
				for(uint16_t i=start;i<width;i++) substr += str[i];
				return substr;
			}
		public:
			#pragma GCC diagnostic push
			#pragma GCC diagnostic ignored "-Wignored-qualifiers" // silence this warning, the qualifiers are necesarry
			static const constexpr inline uint16_t __get_op_size() { return op_size; }
			#pragma GCC diagnostic pop
			inline uint64_t* __get_op() { return op; }
	
			const constexpr static uint16_t size = bitsize;
			template<uint8_t base=0> // type of input (int = base 10, hex = base 16)
			BigUint(std::string input);
			template<uint8_t base=0> // type of input (int = base 10, hex = base 16)
			BigUint(const char *input);

			// assign uint64_t compile time array to op, there is also a non-garunteed compile time function. That is a constructor
			template<uint16_t len, std::array<uint64_t, len> tmp_op>
			consteval BigUint assign_op() noexcept;

			// the next constructor as a compile-time function
			template<typename ...Ts>
			//consteval BigUint assign_conste(const __uint128_t&&input1, Ts&&...input) noexcept; // assign consteval using the same method as the next function
			consteval BigUint assign_conste(Ts...input) noexcept; // assign consteval using the same method as the next function
	
			// numerical input. If number is 256-bit, input = left 128-bit, right 128-bit
			constexpr explicit BigUint(const uint16_t count, __uint128_t input...);
	
			// input as operation array
			constexpr explicit BigUint(uint64_t *input, uint16_t len);
	
			// decleration
			inline constexpr BigUint() = default;
	
			// assign uint256 to another uint256
			//BigUint operator=(const BigUint &num);
			constexpr BigUint operator=(const char *&num);
	
			// arithmetic operations
			constexpr BigUint operator+(const BigUint &num);
			constexpr BigUint operator+=(const BigUint &num);
			constexpr BigUint operator-(const BigUint &num);
			constexpr BigUint operator-=(const BigUint &num);
			constexpr BigUint operator*(const BigUint &num);
			constexpr BigUint operator*=(const BigUint &num);
			constexpr BigUint operator/(const BigUint &num);
			constexpr BigUint operator/=(const BigUint &num);
			constexpr BigUint operator++();
			constexpr BigUint operator--();
			constexpr bool operator[](const uint32_t &index) const; // access specific bit of the number
			constexpr uint64_t operator[](const uint16_t &index) const; // access specific 64-bit op index
	
			// bitwise operators
			constexpr BigUint operator~() const;
			constexpr BigUint operator&(const BigUint &num);
			constexpr BigUint operator&=(const BigUint &num);
			constexpr BigUint operator^(const BigUint &num);
			constexpr BigUint operator^=(const BigUint &num);
			constexpr BigUint operator>>(const BigUint &num);
			constexpr BigUint operator>>=(const BigUint &num);
			constexpr BigUint operator<<(const BigUint &num);
			constexpr BigUint operator<<=(const BigUint &num);
			constexpr BigUint operator|(const BigUint &num);
			constexpr BigUint operator|=(const BigUint &num);
	
			// boolean operators
			constexpr bool operator&&(const BigUint &num);
			constexpr bool operator||(const BigUint &num);
			constexpr bool operator==(const BigUint &num);
			constexpr bool operator!();
			constexpr bool operator!=(const BigUint &num);
			constexpr bool operator<(const BigUint &num);
			constexpr bool operator<=(const BigUint &num);
			constexpr bool operator>(const BigUint &num);
			constexpr bool operator>=(const BigUint &num);
			
			// delete operators for deleting run-time objects
			inline void operator delete(void *dat); // delete object itself
	
			// implicit conversion operators
			inline constexpr operator __uint128_t() const noexcept;
	
			inline constexpr operator uint64_t*() const noexcept { return op; }
	
			template<uint16_t n> friend std::ostream& operator<<(std::ostream& cout, BigUint<n> toprint);
	
		protected:
			// remove 0x if starting with 0x
			constexpr inline bool rm_trailhex(const char *&num)
			{
				std::string_view str(const_cast<char*>(num), 2);
				if (str == "0x") {
					num += 2; // delete the 0x
					return 1;
				}
				return 0;
			} 
	
			// check if number is hex
			constexpr bool is_hex(const char *num, size_t numlen)
			{
				for(size_t i=0;i<numlen;i++)
					if(not isxdigit(*(num+i))) return 0;
				return 1;
			}
	
			// check if input is base16
			constexpr bool input_hex(const char *&input, size_t input_len)
			{
				// check if input is hex
				bool _is_hex = rm_trailhex(input); // remove trailing character if it exists
				if(!_is_hex) { // if no hex trail character '0x'
					_is_hex = is_hex(input, input_len); // check if input is hex
					return _is_hex;
				} else {
					return 1;
				}
			}
	
			// convert string to bigint
			template<uint8_t base=0> // type of input (int = base 8, hex = base 16)
			constexpr void strtobigint(const char *input)
			{
				constexpr const bool base8 = base==8;
				constexpr const bool base16 = base==16;
	   			const size_t len = strlen(input);
				// TODO: check if input length is in range of operator array length, if it's not generate compile time warning
	
				if constexpr(base16 or base8) {
					if constexpr(base16) {
						rm_trailhex(input); // remove trailing character if it exists
						hexoct_to_bigint(input, len, 16);
					} else { // base 8
						hexoct_to_bigint(input, len, 8);
					}
				} else {
	   				bool hex_input = input_hex(input, len);
					if(hex_input) hexoct_to_bigint(input, len, 16);
					else throw wrong_type_error("string or const char* input has to be hex");
				}
			}
			
			// helper algorithm to convert hex or oct values to big integer
			constexpr void hexoct_to_bigint(const char *input, uint16_t len, const unsigned char part_size)
			{
	   			// convert oct/hex input to op elements
	   			const uint8_t ind = len%part_size;
	   			const uint16_t multiple16_count = (len-ind)/part_size;
				uint64_t *tmp;
				if(multiple16_count != 0) {
	   				tmp = new uint64_t[multiple16_count];
					// get's the first multiple of part_size values of the integer
	   				for(uint16_t i=0;i<multiple16_count;i++) {
						std::stringstream ss;
						ss << std::hex << get_substring(input, i*part_size+ind,part_size);
						ss >> tmp[i];
					}
	   			   	if(ind!=0) { // if len not a multiple of part_size
	   			   		op_nonleading_i = op_size-multiple16_count-1; // includes the final value
						std::stringstream ss;
						ss << std::hex << get_substring(input, 0,ind);
						ss >> op[op_size-multiple16_count-1];
	   			   	 	for(uint16_t i=multiple16_count;i>0;i--) op[op_size-i] = tmp[multiple16_count-i];
	   			   	} else {
	   			   		op_nonleading_i = op_size-multiple16_count; // if length is a multiple of part_size
	   			   	 	for(uint16_t i=multiple16_count;i>0;i--) op[op_size-i+1] = tmp[multiple16_count-i];
	   			   	}
				} else { // length < part_size
	   				tmp = new uint64_t[1];
					std::stringstream ss;
					ss << std::hex << input;
					ss >> *tmp;
	   			   op_nonleading_i = op_size-1;
				   op[op_nonleading_i] = *tmp;
				}
	   			// pad the operator array
	   			for(uint16_t i=0;i<op_nonleading_i;i++) op[i] = 0x0000000000000000ULL;
	   			delete[] tmp;
			}
	};
	 
	// output stream operator
	template<uint16_t bitsize>
	std::ostream& operator<<(std::ostream& cout, BigUint<bitsize> toprint)
	{
		bool pad_stopped = 0; // if pad stopped, then print the rest, including zero values
		bool last_num = 0;
		uint8_t pad_size;
	
		// initialize the pad sizes based on whether the ostream is dec, hex, oct, bin.
		std::ios_base::fmtflags fmt = cout.flags();
		if(fmt & std::ios_base::dec) pad_size = 20;
		else if(fmt & std::ios_base::hex) pad_size = 16; // pad count: 2^64-1=16 base 16 digits
		else if(fmt & std::ios_base::oct) pad_size = 22;
		else pad_size = 64; // bin
		for(uint16_t i=0;i<toprint.op_size;i++) {
			if(toprint.op[i] != 0x0000000000000000ULL) pad_stopped=1;
			if(pad_stopped) {
				if(last_num)
					cout << std::setfill('0') << std::setw(pad_size) << toprint.op[i];
				else
					cout << toprint.op[i]; // no padding
				last_num = 1; // if not first print, pad
			}
		}
		if(!pad_stopped) // if zero
			cout << "0";
		return cout;
	}
	 
	 
	// input stream operator
	template<size_t bitsize>
	std::istream& operator>>(std::istream& cin, BigUint<bitsize> &input)
	{
		std::string num;
		cin >> num;
		input = num;
		return cin;
	}
	
	using uint192_t = BigUint<192>;
	using uint256_t = BigUint<256>;
	using uint384_t = BigUint<384>;
	using uint512_t = BigUint<512>;
}; /* NAMESPACE BIGINT */

// include here because of template class and function
#include "bigint.cpp"

#endif /* BIGINT_H */
