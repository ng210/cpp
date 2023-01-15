#include <stdio.h>
#include "utils/utils.h"

NS_FW_BASE_BEGIN

size_t Utils::randomSeed_ = 1;
size_t Utils::randomSeed2_ = 0;
const size_t Utils::prime1_ = 127513;
const size_t Utils::prime2_ = 0x3cb7a7f6d295ef7ULL;	// 32487;	// 0xa0000000000002ULL;
const size_t Utils::prime3_ = 175892;


size_t Utils::random_() {
	Utils::randomSeed_ = Utils::randomSeed_ * Utils::prime1_ + Utils::prime2_;
	Utils::randomSeed2_ = Utils::randomSeed2_ * Utils::prime3_ + Utils::prime2_;
	return Utils::randomSeed_ + Utils::randomSeed2_;

}

size_t Utils::random(size_t range) {
	return Utils::random_() % range;
}

double Utils::random() {
	union {
		double d;
		size_t ull;
	};
	ull = Utils::random_() & 0x0fffffffffffffULL | 0x3ff0000000000000ULL;
	return d - 1.0;
}

double Utils::randomSigned() {
	union {
		double d;
		size_t ull;
	};
	ull = Utils::random_() & 0x0fffffffffffffULL | 0x4000000000000000ULL;
	return d - 3.0;
}

NS_FW_BASE_END