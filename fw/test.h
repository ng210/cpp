#ifndef __TEST_H
#define __TEST_H

#include "console/consoleapp.h"
#include "basedef.h"

NS_FW_BASE_BEGIN

//#define TEST(t) t() ? failed++ : passed++;
//#define assert(label, expr) INFO(" - " label ": "); if (expr) { passed++; INFO("passed\n"); } else { failed++; INFO("failed\n"); };
//#define HEADER(text) LOG("** " text "\n"); int passed = 0, failed = 0
//#define RESULTS() LOG("    Results: %d/%d=%.02f%%\n\n", passed, (passed + failed), (100.0f*passed) / (passed + failed))

class Test;

typedef void (Test::*TestMethod)();

class Test
{
public:
    Test();
    virtual ~Test();
    int passed_;
    int failed_;
    int totalPassed_;
    int totalFailed_;
    virtual void setup();
    virtual void tearDown();
    void test(const char* label, TestMethod testMethod);
    void displayFinalResults(const char* label = NULL);
    void assert(const char* label, bool condition);
    virtual void runAll(int& totalPassed, int& totalFailed) = 0;

    static void setColor(float ratio);
    static Console* cons;
};

NS_FW_BASE_END

#endif