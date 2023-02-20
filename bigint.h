#ifndef BIGINT_H

#include <stdint.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <exception>

class wrong_type_error : public std::runtime_error {
	explicit wrong_type_error(const std::string& str) : std::runtime_error(str) {}
};

// custom-size integer class
template<uint16_t bitsize>
class NewInt {
	public:
		// operator array
		const static constexpr uint16_t op_size = bitsize/64;
		uint64_t op[op_size]; // when iterating, start from end to start

		NewInt(std::string input)
		{
			bool valid_input = input_valid();	
			const uint16_t len = input.length();
			if (valid_input) {
				// convert hex input to op elements
				const uint16_t multiple16_count = len/16;
				std::string substr;
				uint64_t *tmp = new uint64_t[multiple16_count];
				for(uint16_t i=0;i<multiple16_count;i++) {
					substr = input.substr(i*16,16);
					tmp[i] = static_cast<uint64_t>(std::stoull(substr));
				}
				for(uint64_t i=multiple16_count;i>=0;i++) op[op_size-i-1] = tmp[i];
				delete[] tmp;

				// get the last smaller-than-16-byte end of input
			} else {
				throw wrong_type_error("input has to be hexadecimal");
			}
		}

		// numerical input. If number is 256-bit, input = left 128-bit, right 128-bit
		template<size_t count> // number of arguements
		NewInt(__uint128_t input, ...) {
			// uint128_t input to 2 uint64_t integers
		    static constexpr const __uint128_t bottom_mask = (__uint128_t{1} << 64) - 1; // 0x0000000000000000ffffffffffffffffU
		    static constexpr const __uint128_t top_mask = ~bottom_mask; 				 // 0xffffffffffffffff0000000000000000U
			for(uint8_t i=0;i<count;i++) {
				__uint128_t num = *((__uint128_t*)&input + i);
				op[op_size-i-1] = num&bottom_mask;
				op[op_size-i-2] = (num&top_mask) >> 64;
			}
		}

		NewInt(uint64_t* input, uint16_t len) // input order has to be: input[0] = most left 64-bit
		{
			// add input to operator array
			for(uint16_t i=0;i<len;i++) op[i] = input[i];

			// pad operator array
			uint16_t pad_count = op_size-len;
			for(uint16_t i=len;i<pad_count;i++) op[i] = 0x0000000000000000ULL;
		}

		constexpr NewInt() = default;

		// assign uint256 to another uint256
		NewInt operator=(const NewInt &num);

		// arithmetic operations
		NewInt operator+(const NewInt &num);
		NewInt operator+=(const NewInt &num);
		NewInt operator-(const NewInt &num);
		NewInt operator-=(const NewInt &num);
		NewInt operator*(const NewInt &num);
		NewInt operator*=(const NewInt &num);
		NewInt operator/(const NewInt &num);
		NewInt operator/=(const NewInt &num);

		// bitwise operators
		NewInt operator~() const;
		NewInt operator&(const NewInt &num);
		NewInt operator&=(const NewInt &num);
		NewInt operator^(const NewInt &num);
		NewInt operator^=(const NewInt &num);
		NewInt operator>>(const NewInt &num);
		NewInt operator>>=(const NewInt &num);
		NewInt operator<<(const NewInt &num);
		NewInt operator<<=(const NewInt &num);
		NewInt operator|(const NewInt &num);
		NewInt operator|=(const NewInt &num);

		// boolean operators
		NewInt operator&&(const NewInt &num);
		NewInt operator||(const NewInt &num);
		NewInt operator==(const NewInt &num);
		NewInt operator!();
		NewInt operator!=(const NewInt &num);
		NewInt operator<(const NewInt &num);
		NewInt operator<=(const NewInt &num);
		NewInt operator>(const NewInt &num);
		NewInt operator>=(const NewInt &num);

	protected:
		// return carry on a 64-bit number
		inline uint64_t carry_size64_add(uint64_t num)
		{
			return UINT64_MAX - num;
		}

		// check if string is base 10 integer
		bool check_num(std::string num)
		{
			for(uint8_t ch : num) {
				if(not isdigit(ch)) return 0;
			}
			return 1;
		}

		// remove 0x if starting with 0x
		inline bool rm_trailhex(std::string &num) {
			if (num.starts_with("0x")) {
				num = num.erase(0,2);
				return 1;
			}
			return 0;
		} 

		// check if number is hex
		bool is_hex(std::string num) {
			for(uint8_t ch : num) {
				if(not isxdigit(ch)) return 0;
			}
			return 1;
		}

		// number to hex
		template<typename T>
		inline std::string hex(T num)
		{
			std::stringstream ss;
			ss << std::hex << num;
			return ss.str();
		}

		// op to hex
		std::string hex()
		{
			std::stringstream ss;
			for(uint16_t i=0;i<op_size;i++) {
				ss << std::hex << op[i];
			}
			return ss.str();
		}

		// check if input is base16
		bool input_valid(std::string &input) {
			// check if input is hex
			bool _is_hex = rm_trailhex(input); // remove trailing character if it exists
			if(!_is_hex) { // if no hex trail character '0x'
				_is_hex = is_hex(input); // check if input is hex
			} else {
				return 1;
			}
			if(!_is_hex) {
				return 0;
			} else {
				return 1;
			}
		}
};
 
// output stream operator
template<size_t bitsize>
std::ostream& operator<<(std::ostream& out, NewInt<bitsize> toprint)
{
	for(uint16_t i=0;i<toprint.op_size;i++) out << toprint.op[i];
	return out;
}
 
// input stream operator
//template<size_t bitsize>
//std::istream& NewInt::operator>>(std::istream& out, NewInt toprint);

// declare class uint256
class uint256_t : public NewInt<256>
{
	public:
		uint256_t(std::string num); // num: uint256 number as base 10 string
		inline uint256_t(uint64_t *num); // num: uint256 number as array
		inline uint256_t(__uint128_t num); // numerical uint128_t input
		inline uint256_t(); // no input declaration (num=0)
};

#endif /* BIGINT_H */
