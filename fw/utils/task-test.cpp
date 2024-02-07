#include "utils/task-test.h"
#include "base/memory.h"
#include "base/debug.h"

MyTask::MyTask(ACTION* action) : Task(action) {
	ix = 0;
	numbers_ = MALLOC(int, 100);
}

MyTask::~MyTask() {
	FREE(numbers_);
}

DWORD MyTask::taskProc(MyTask* task) {
	task->numbers_[task->ix] = task->ix;
	task->ix++;
	Sleep(20);
	return (DWORD)(task->ix == 100);
}

DWORD TaskTest::testProc_(void* context) {
	var n = *(int*)context;
	n++;
	*(int*)context = n;
	Sleep(10);
	return (DWORD)(n == 50 ? 1 : 0);
}

void TaskTest::taskTestBasic() {
	int number = 0;
	int result = 0;
	var task = NEW_(Task, &TaskTest::testProc_, &number, true);
	assert("Should be in ready state", task->state() == TASK_STATE::TaskReady);
	task->run();
	assert("Should be in running state", task->state() == TASK_STATE::TaskRunning);
	LOG("Wait for 0.5 second\n");
	Sleep(500);
	task->stop();
	assert("Should be in interrupted state", task->state() == TASK_STATE::TaskInterrupted);
	assert("Should have changed the value", number > 0);
	LOG("number is %d\n", number);
	task->reset()->run();
	LOG("Wait for 2 seconds\n");
	Sleep(2000);
	assert("Should be in completed state", task->state() == TASK_STATE::TaskCompleted);
	assert("Should have the final value", number == 50);
	DEL_(task);
}

void TaskTest::taskTestWait() {
	int number = 0;
	int result = 0;
	var task = NEW_(Task, &TaskTest::testProc_, &number, true);
	assert("Should be in ready state", task->state() == TASK_STATE::TaskReady);
	task->run();
	assert("Should be in running state", task->state() == TASK_STATE::TaskRunning);
	LOG("Wait for task to complete\n");
	task->wait();
	assert("Should have the final value", number == 50);
	assert("Should be in completed state", task->state() == TASK_STATE::TaskCompleted);
	DEL_(task);
}

void TaskTest::taskTestNoContext() {
	var task1 = NEW_(MyTask, (ACTION*)&MyTask::taskProc);
	var task2 = NEW_(MyTask, (ACTION*)&MyTask::taskProc);
	assert("Should be in ready state", task1->state() == TASK_STATE::TaskReady);
	assert("Should be in ready state", task2->state() == TASK_STATE::TaskReady);
	task1->run();
	task2->run();
	assert("Should be in running state", task1->state() == TASK_STATE::TaskRunning);
	LOG("Wait for 1 second\n");
	Sleep(1000);
	task1->stop();
	LOG("task1.ix=%d, task2.ix=%d\n", task1->ix, task2->ix);
	assert("Should be in interrupted state", task1->state() == TASK_STATE::TaskInterrupted);
	assert("Should have advanced the index", task1->ix > 0);
	var ii = 0;
	while (task1->numbers()[ii] == ii) ii++;
	cons->dump((byte*)task1->numbers(), 4 * task1->ix, 32);
	assert("Should have filled memory", task1->ix == ii);
	LOG("Wait for task2 to complete\n");
	task2->wait();
	assert("Should be in completed state", task2->state() == TASK_STATE::TaskCompleted);
	ii = 0;
	while (task2->numbers()[ii] == ii) ii++;
	cons->dump((byte*)task2->numbers(), 4 * task2->ix, 32);
	assert("Should have filled memory", task2->ix == ii);
	DEL_(task1);
	DEL_(task2);
}

void TaskTest::runAll(int& totalPassed, int& totalFailed) {
	LOG(" * Task tests\n");
	test("Task basic tests", (TestMethod)&TaskTest::taskTestBasic);
	test("Task wait test", (TestMethod)&TaskTest::taskTestWait);
	test("Task with own context", (TestMethod)&TaskTest::taskTestNoContext);
	totalPassed += totalPassed_;
	totalFailed += totalFailed_;
}
