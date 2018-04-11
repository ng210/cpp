#ifndef __UTILS_H
#define __UTILS_H

#include "basedef.h"

NS_FW_BASE_BEGIN

class Utils {
	static size_t randomSeed_;
	static size_t randomSeed2_;
	static const size_t prime1_;
	static const size_t prime2_;
	static const size_t prime3_;
	static size_t random_();
public:
	static double random();
	static double randomSigned();
	static size_t random(size_t);
};

NS_FW_BASE_END

#endif