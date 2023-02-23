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
class BigInt {
	protected:

		// operator array
		constexpr const static uint16_t op_size = bitsize%64==0 ? bitsize/64 : bitsize/64+1;
		uint64_t op[op_size]; // when iterating, start from end to start
		uint16_t op_nonleading_i; // index of op when leading zeros end
	public:
		const constexpr static uint16_t size = bitsize;
		BigInt(std::string input);

		// numerical input. If number is 256-bit, input = left 128-bit, right 128-bit
		template<size_t count> // number of arguements
		BigInt(__uint128_t input, ...);

		// input as operation array
		BigInt(uint64_t *input, uint16_t len);

		// decleration
		constexpr BigInt() = default;

		// destructors
		~BigInt();

		// assign uint256 to another uint256
		BigInt operator=(const BigInt &num);

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
		BigInt operator&&(const BigInt &num);
		BigInt operator||(const BigInt &num);
		BigInt operator==(const BigInt &num);
		BigInt operator!();
		BigInt operator!=(const BigInt &num);
		BigInt operator<(const BigInt &num);
		BigInt operator<=(const BigInt &num);
		BigInt operator>(const BigInt &num);
		BigInt operator>=(const BigInt &num);

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
std::ostream& operator<<(std::ostream& cout, BigInt<bitsize> &toprint)
{
	bool pad_stopped = 0; // if pad stopped, then print the rest, including zero values
	for(uint16_t i=0;i<toprint.op_size;i++) {
			if(toprint.op[i] != 0x0000000000000000ULL) pad_stopped=1;
			if(pad_stopped)
				cout << std::setfill('0') << std::setw(20) << toprint.op[i]; // pad count: 2^64-1=20 base 10 digits
	}
	return cout;
}
 
// input stream operator
template<size_t bitsize>
std::istream& operator>>(std::istream& cin, BigInt<bitsize> &input)
{
	std::string num;
	cin >> num;
	input(num);
	return cin;
}

// declare class uint256
class uint256_t : public BigInt<256>
{
	public:
		uint256_t(std::string num); // num: uint256 number as base 10 string
		inline uint256_t(uint64_t *num); // num: uint256 number as array
		template<size_t count> inline uint256_t(__uint128_t num, ...); // numerical uint128_t input
		inline constexpr uint256_t(); // no input declaration (num=0)
};

#endif /* BIGINT_H */
