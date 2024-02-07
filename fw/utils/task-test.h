#ifndef __TASK_TEST_H
#define __TASK_TEST_H

#include "test.h"
#include "utils/task.h"
#include "collection/collection.h"

NS_FW_BASE_USE

class MyTask : public Task {
	PROP_R(int*, numbers);
public:
	MyTask(ACTION* action);
	~MyTask();

	int ix;

	static DWORD taskProc(MyTask*);
};

class TaskTest : public Test {
public:
	void taskTestBasic();
	void taskTestWait();
	void taskTestNoContext();
	void runAll(int& totalPassed, int& totalFailed);

	static DWORD testProc_(void*);
};

#endif