#ifndef BIGINT_H

#include <stdint.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <exception>

class wrong_type_error : public std::runtime_error {
	public:
		explicit wrong_type_error(const std::string& str) : std::runtime_error(str) {}
};

// custom-size integer class
template<uint16_t bitsize>
class BigInt {
	protected:

		// operator array
		constexpr const static uint16_t op_size = bitsize%64==0 ? bitsize/64 : bitsize/64+1;
		uint64_t op[op_size]; // when iterating, start from end to start
		uint16_t op_nonleading_i; // index of op when leading zeros end
	public:
		const constexpr static uint16_t size = bitsize;
		template<char base=0> // type of input (int = base 10, hex = base 16)
		BigInt(const char *input);
		template<char base=0> // type of input (int = base 10, hex = base 16)
		BigInt(std::string input);

		// numerical input. If number is 256-bit, input = left 128-bit, right 128-bit
		template<size_t count> // number of arguements
		explicit BigInt(__uint128_t input, ...);

		// input as operation array
		explicit BigInt(uint64_t *input, uint16_t len);

		// decleration
		inline constexpr BigInt() = default;

		// assign uint256 to another uint256
		//BigInt operator=(const BigInt &num);
		BigInt operator=(const std::string &num);

		// arithmetic operations
		BigInt operator+(const BigInt &num);
		BigInt operator+=(const BigInt &num);
		BigInt operator-(const BigInt &num);
		BigInt operator-=(const BigInt &num);
		BigInt operator*(const BigInt &num);
		BigInt operator*=(const BigInt &num);
		BigInt operator/(const BigInt &num);
		BigInt operator/=(const BigInt &num);
		BigInt operator++();
		BigInt operator--();
		bool operator[](const uint32_t &index) const; // access specific bit of the number
		uint64_t operator[](const uint16_t &index) const; // access specific 64-bit op index

		// bitwise operators
		BigInt operator~() const;
		BigInt operator&(const BigInt &num);
		BigInt operator&=(const BigInt &num);
		BigInt operator^(const BigInt &num);
		BigInt operator^=(const BigInt &num);
		BigInt operator>>(const BigInt &num);
		BigInt operator>>=(const BigInt &num);
		BigInt operator<<(const BigInt &num);
		BigInt operator<<=(const BigInt &num);
		BigInt operator|(const BigInt &num);
		BigInt operator|=(const BigInt &num);

		// boolean operators
		bool operator&&(const BigInt &num);
		bool operator||(const BigInt &num);
		bool operator==(const BigInt &num);
		bool operator!();
		bool operator!=(const BigInt &num);
		bool operator<(const BigInt &num);
		bool operator<=(const BigInt &num);
		bool operator>(const BigInt &num);
		bool operator>=(const BigInt &num);

		template<uint16_t n> friend std::ostream& operator<<(std::ostream& cout, BigInt<n> toprint);

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
		inline bool rm_trailhex(std::string &num)
		{
			if (num.starts_with("0x")) {
				num = num.erase(0,2);
				return 1;
			}
			return 0;
		} 

		// check if number is hex
		bool is_hex(std::string num)
		{
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
		bool input_hex(std::string &input)
		{
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

#pragma GCC diagnostic ignored "-Wtype-limits"
		template<char base=0> // type of input (int = base 10, hex = base 16)
		void strtobigint(std::string input)
		{
			unsigned char part_size;
			constexpr const bool base10 = base==10;
			constexpr const bool base16 = base==16;
			if constexpr(base10 or base16) {
				if constexpr(base10) {
					part_size=20;
				} else {
					part_size=16;
				}
			} else {
				const bool int_input = check_num(input);
				if(int_input) {
					// if base 10, part-size = len(str(2**64-1))
					part_size=20;
				} else {
   					bool hex_input = input_hex(input);
					// if base 16, part-size = len(hex(2**64-1))
					if(hex_input) part_size=16;
					else throw wrong_type_error("input has to be int or hex");
				}
			}
   			const uint16_t len = input.length();

   			// convert int/hex input to op elements
   			const uint8_t ind = len%part_size;
   			const uint16_t multiple16_count = (len-ind)/part_size;
			uint64_t *tmp;
			if(multiple16_count != 0) {
   				tmp = new uint64_t[multiple16_count];
   				for(uint16_t i=0;i<multiple16_count;i++)
   				    tmp[i] = static_cast<uint64_t>(std::stoull(input.substr(i*part_size+ind,part_size)));
   			   if(ind!=0) {
   			   	op_nonleading_i = op_size-multiple16_count+1;
   			    	op[multiple16_count] = static_cast<uint64_t>(std::stoull(input.substr(0,ind)));
   			    	for(uint16_t i=multiple16_count;i>0;i--) op[op_size-i-2] = tmp[i];
   			   } else {
   			   	op_nonleading_i = op_size-multiple16_count; // if length is a multiple of part_size
   					for(uint16_t i=multiple16_count;i>0;i--) op[op_size-i-1] = tmp[i];
   			   }
			} else { // length < 16
   				tmp = new uint64_t[1];
   				tmp[0] = static_cast<uint64_t>(std::stoull(input));
   			   op_nonleading_i = op_size-1;
			   op[op_nonleading_i] = *tmp;
			}
   			// pad the operator array
   			for(uint16_t i=0;i<op_nonleading_i;i++) op[i] = 0x0000000000000000ULL;
   			delete[] tmp;
			for(uint16_t i=op_size-1;i>op_size-4;i--) std::cout << op[i] << " ";
		}
#pragma GCC diagnostic pop
};
 
// output stream operator
template<uint16_t bitsize>
std::ostream& operator<<(std::ostream& cout, BigInt<bitsize> toprint)
{
	bool pad_stopped = 0; // if pad stopped, then print the rest, including zero values
	bool last_num = 0;
	for(uint16_t i=0;i<toprint.op_size;i++) {
			if(toprint.op[i] != 0x0000000000000000ULL) pad_stopped=1;
			if(pad_stopped) {
				if(last_num)
					cout << std::setfill('0') << std::setw(20) << toprint.op[i]; // pad count: 2^64-1=20 base 10 digits
				else
					cout << toprint.op[i]; // no padding
				last_num = 1; // if first print, don't have padding
			}
	}
	if(!pad_stopped) // if zero
		cout << "0";
	return cout;
}
 
 
// input stream operator
template<size_t bitsize>
std::istream& operator>>(std::istream& cin, BigInt<bitsize> &input)
{
	std::string num;
	cin >> num;
	input = num;
	return cin;
}

using uint192_t = BigInt<192>;
using uint256_t = BigInt<256>;
using uint384_t = BigInt<384>;
using uint512_t = BigInt<512>;

#endif /* BIGINT_H */
