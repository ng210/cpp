#include "test.h"
#include <stdio.h>
#include "base/debug.h"

NS_FW_BASE_USE

Test::Test() {
    passed_ = 0;
    failed_ = 0;
    totalPassed_ = 0;
    totalFailed_ = 0;
}

Test::~Test() {}

void Test::setup() {
}

void Test::tearDown() {
}

void Test::assert(const char* label, bool condition) {
    char str[1024];
    sprintf_s(str, 1023, " - %s: %s\n", label, (condition ? "passed" : "failed"));
    LOG(str);
    if (condition) {
        totalPassed_++;
        passed_++;
    }
    else {
        totalFailed_++;
        failed_++;
    }
}

void Test::test(const char* label, TestMethod testMethod) {
    LOG("%s\n", label);
    passed_ = 0; failed_ = 0;
    (this->*testMethod)();
    LOG(" Results: %d/%d=%.02f%%\n\n", passed_, (passed_ + failed_), (100.0f * passed_) / (passed_ + failed_));
}

void Test::displayFinalResults() {
    LOG("\n\n*** Final results: %d/%d=%.02f%%\n\n", totalPassed_, (totalPassed_ + totalFailed_), (100.0f * totalPassed_) / (totalPassed_ + totalFailed_));
}
