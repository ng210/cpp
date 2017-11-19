#include "base/utils.h"

NS_FW_BASE_BEGIN

size_t Utils::randomSeed_ = 1;
const size_t Utils::prime1_ = 0x80000000000003ULL;
const size_t Utils::prime2_ = 0xa0000000000002ULL;

size_t Utils::random(size_t range) {
	size_t value = prime1_ + prime2_ * randomSeed_;
	Utils::randomSeed_ += value;
	return value % range;
}

double Utils::random() {
	union {
		double d;
		size_t ull;
	};
	ull = prime1_ + prime2_ * randomSeed_;
	Utils::randomSeed_ += ull;
	ull = ull & 0x0fffffffffffffULL | 0x3ff0000000000000ULL;
	return d - 1.0;
}

double Utils::randomSigned() {
	union {
		double d;
		size_t ull;
	};
	ull = prime1_ + prime2_ * randomSeed_;
	Utils::randomSeed_ += ull;
	ull = ull & 0x0fffffffffffffULL | 0x4000000000000000ULL;
	return d - 3.0;
}

NS_FW_BASE_END