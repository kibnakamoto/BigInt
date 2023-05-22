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
	// constexpr uint256_t num3 = BigUint<256><1>(256);
	std::cout << "int num: " << num;
	std::cout << "\nhex num2: " << std::hex << num2;
	auto num3 = num + num2;
	std::cout << "\nnum+num2: " << std::hex << num3 << std::endl;
	std::cout << "num - num2: " << (num - num2);
	std::cout << std::endl;
	return 0;
}

