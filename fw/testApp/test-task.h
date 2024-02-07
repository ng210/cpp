#ifndef __TEST_TASK_H
#define __TEST_TASK_H

#include "test.h"
#include "basedef.h"
#include "utils/task.h"
#include "win/staticctrl.h"

NS_FW_BASE_BEGIN
NS_FW_WIN_USE

class TestTask : public Task {
	StaticCtrl* ctrl_;
	TestApp* app_;
	int dx_;
	int counter_;
	
	static DWORD taskProc(TestTask*);
public:
	TestTask(TestApp*);
	~TestTask();

};

NS_FW_BASE_END

#endif