#define TEST(t) t() ? failed++ : passed++;
#define ASSERT(label, expr) INFO(" - " label ": "); if (expr) { passed++; INFO("passed\n"); } else { failed++; INFO("failed\n"); };
#define HEADER(text) LOG("** " text "\n"); int passed = 0, failed = 0
#define RESULTS() LOG("    Results: %d/%d=%.02f%%\n\n", passed, (passed + failed), (100.0f*passed) / (passed + failed))
