#include "test.h"
#include "base/debug.h"

NS_FW_BASE_USE

Console* Test::cons = NULL;

Test::Test() {
    passed_ = 0;
    failed_ = 0;
    totalPassed_ = 0;
    totalFailed_ = 0;

    if (Test::cons == NULL) {
        Test::cons = getConsole();
    }
}

Test::~Test() {}

void Test::setup() {
}

void Test::tearDown() {
}

void Test::assert(const char* label, bool condition) {
    Test::cons->setcolor(ConsoleColors::gray);
    Test::cons->printf(" - %s: ", label);
    if (condition) {
        Test::cons->setcolor(ConsoleColors::green);
        Test::cons->printf(" passed\n");
    }
    else {
        Test::cons->setcolor(ConsoleColors::red);
        Test::cons->printf(" failed\n");
    }
    Test::cons->setcolor(ConsoleColors::gray);

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
    Test::cons->setcolor(ConsoleColors::gray);
    Test::cons->printf("\n *** %s\n", label);
    passed_ = 0; failed_ = 0;
    (this->*testMethod)();
    var sum = passed_ + failed_;
    var ratio = (100.0f * passed_) / sum;
    Test::cons->printf(" *** %s Results: ", label);
    Test::setColor(ratio);
    Test::cons->printf(" %d/%d = %.02f%%\n", passed_, sum, ratio);
    Test::cons->setcolor(ConsoleColors::gray);
}

void Test::displayFinalResults(const char* label) {
    if (label == NULL) label = "Final results:";
    Test::cons->setcolor(ConsoleColors::gray);
    Test::cons->printf(" ### %s ", label);
    var sum = totalPassed_ + totalFailed_;
    var ratio = (100.0f * totalPassed_) / sum;
    Test::setColor(ratio);
    Test::cons->printf("%d/%d=%.02f%%\n\n", totalPassed_, sum, ratio);
    Test::cons->setcolor(ConsoleColors::gray);
}

void Test::setColor(float ratio) {
    if (ratio < 60.f) Test::cons->setcolor(ConsoleColors::red);
    else if (ratio < 90.f) Test::cons->setcolor(ConsoleColors::yellow);
    else Test::cons->setcolor(ConsoleColors::green);
}