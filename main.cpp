// main file of BigInt, a demo of how to use library
#include <iostream>
#include <string.h>

#include "bigint.h"

using namespace BigInt;

int main()
{
	// uint256_t num="256";
	// uint256_t num2 = std::string("256");
	uint256_t num="2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
	uint256_t num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
	std::cout << "int num: " << num;
	std::cout << "\nhex num2: " << std::hex << num2;
	auto num3 = num + std::string("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffff") ;//+ num2;
	//uint192_t num4 = std::string("134395843534503845740957390690572"); // 132-bit hex
	uint192_t num4 = std::string("13439584353450384574095739069057212345678"); // 132-bit hex
	std::cout << "\nnum+num2: " << std::hex << num3 << std::endl;
	std::cout << "\nhex num4: " << std::hex << num4;

	std::cout << "\nnum - num4: " << (num-num4.to<256>());
	std::cout << "\n~num: " << ~num;
	std::cout << "\nnum << 20: " << (num << uint256_t(20));
	std::cout << "\nnum * 200: " << num.to<256>() * uint256_t(200);
	std::cout << "\nnum / 2: " << (num / uint256_t(2));
	std::cout << "\nnum % 20000: " << (num % uint256_t(20000));
	std::cout << std::endl;
	return 0;
}

