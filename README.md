# Big Integer Library in C++

The library is optmized for C++23

addition and basic operators work in nanoseconds

Around 11 constructors and functions for input. Including Consteval input. Use the test file for reference.

Garunteed to work for the following supported bitsizes: 2^x     e.g. 64, 128, 256

When dealing with numbers smaller than or equal to 2^32768, use BigUint\<bitsize\>.

When dealing with numbers smaller than or equal to 2^2147483648, use LargeUint\<bitsize\>.

When dealing with numbers smaller than or equal to 2^9223372036854775808, use HugeUint\<bitsize\>.

When dealing with numbers smaller than or equal to 2^170141183460469231731687303715884105728, use YugeUint\<bitsize\>.

The bitsize refers to the bit size of the integer. For example, 256-bit int has a bit size of 256. It can be declared as one of the above:

```
BigUint<256> num;
LargeUint<256> num;
HugeUint<256> num;
YugeUint<256> num;
```

Works fastest for 512-bit numbers. So using 512-bit numbers are faster than using 256-bit numbers
