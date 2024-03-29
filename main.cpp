 /* Copyright (c) 2022 Taha
  * this program is free software: you can redistribute it and/or modify
  * it under the terms of the gnu general public license as published by
  * the free software foundation, either version 3 of the license, or
  * (at your option) any later version.
  * this program is distributed in the hope that it will be useful,
  * but without any warranty; without even the implied warranty of
  * merchantability or fitness for a particular purpose.  see the
  * gnu general public license for more details.
  * you should have received a copy of the gnu general public license
  * along with this program.  if not, see <https://www.gnu.org/licenses/>.
  *
  * Author: Taha
  * Date: 2022, Sep 10
  * Main file of BigInt, a demo of how to use library
  */


#include <iostream>
#include <string.h>
#include <sstream>
#include <chrono>

#include "bigint.h"

using namespace BigInt;

int main()
{
	bool error;
	uint256_t num="2337616833552046603458334740849159417653411302789319245661"; // 232-bit hex
	uint256_t num2 = std::string("2337616833552046603458334740849159417653411302789319245661"); // 232-bit hex
	std::cout << "int num: " << std::dec << num; // 949430380321334154025334326121912570021843741522655373338158005769825 // only correct when viewed as segments which are 151252985907061339800714876158150464900052788958496101369789809851389537
	std::cout << "\nhex num2: " << std::hex << num2; // 2337616833552046603458334740849159417653411302789319245661
	auto num3 = num + num2;
	//uint192_t num4 = std::string("134395843534503845740957390690572"); // 132-bit hex
	uint512_t num4 = std::string("13439584353450384574095739069057212345678"); // 132-bit hex
	std::cout << "\nnum+num2: " << std::hex << num3 << std::endl; // 466ec2d066aa408cc068b0668e810922b282eca6822604f1263248acc2
	std::cout << "\nhex num4: " << std::hex << num4; // 13439584353450384574095739069057212345678

	std::cout << "\nnum - num4: " << (num-num4.to<256>()); // 23376168335520465f001edb03ed3f8dd4ea35bdcd8299732106efffe9
	std::cout << "\n~num: " << ~num; // ffffffdcc89e97ccaadfb99fcba7ccb8bf7b6ea6be89acbeecfd876ce6dba99e
	std::cout << "\nnum << 20: " << (num << uint256_t(20)); // 233761683355204660345833474084915941765341130278931924566100000
	std::cout << "\nnum * 200: " << num.to<256>() * uint256_t(200); // 1b834419681a8136fb28e4e80faa67918dbb24710ad6d9ee32eba4637bc8
	std::cout << "\nnum / 2: " << (num / uint256_t(2)); // 119bb0b419aa9023301a2c19a3a04248aca0bb29a089813c498c922b30
	std::cout << "\nnum % 20000: " << (num % uint256_t(20000)); // 2661
	std::cout << "\npow(num, 2): " << pow(num, uint256_t(2)); // b9fe143015ff9a71ef6f1d05068d2b9a565f8dbb28b4c8bdb2b2416d5a6d50c1, answer bigger than 256-bit, so and operator fixes it.
	BigUint<256> a = "2"; // 32768
	std::cout << "\npow: " << pow(a, BigUint<256>(255)); // 8000000000000000000000000000000000000000000000000000000000000000
	std::cout << std::endl;
	std::cout << "random number: " << BigUint<256>::random("23094445503", "322438450683406843002304823", error);
	if(error) {
		// handle error, starting range is bigger than end (from > to)
	}
	std::cout << std::endl;
	//exit(0);

	// log2
	uint512_t num5=8; // 232-bit hex
	auto start1 = std::chrono::high_resolution_clock::now();
	num5 = uint512_t::log2(num5);
	auto end1 = std::chrono::high_resolution_clock::now();
	std::cout << std::endl << num5;
	std::cout << std::endl << "Calculation time: " << std::dec << std::chrono::duration_cast< std::chrono::microseconds >(end1-start1);
	std::cout << std::endl;

	// 2^99,999
	LargeUint<100000> n="2";
	std::cout << "n: " << pow(n, LargeUint<100000>(100000-1)) << std::endl;
	

	// factorial
	LargeUint<1048576> num6=10000; // 1048576-bit hex
	auto start2 = std::chrono::high_resolution_clock::now();
	num6 = num6.factorial();
	auto end2 = std::chrono::high_resolution_clock::now();
	std::cout << std::endl << std::hex << num6;
	std::cout << std::endl << "Calculation time: " << std::dec << std::chrono::duration_cast< std::chrono::milliseconds >(end2-start2);
	std::cout << std::endl;
	return 0;
}

