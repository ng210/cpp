#ifndef __TEST_H
#define __TEST_H


//#define TEST(t) t() ? failed++ : passed++;
//#define ASSERT(label, expr) INFO(" - " label ": "); if (expr) { passed++; INFO("passed\n"); } else { failed++; INFO("failed\n"); };
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
    void displayFinalResults();
    void assert(const char* label, bool condition);
};

#endif