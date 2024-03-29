#ifndef __BASE_TEST_H
#define __BASE_TEST_H

#include "test.h"

NS_FW_BASE_BEGIN

class BaseTest : public Test {
public:
	void strTests();
	void stringTests();
	void streamTests();

	void runAll(int& totalPassed, int& totalFailed);
};

NS_FW_BASE_END

#endif