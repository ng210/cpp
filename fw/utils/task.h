#ifndef __TASK_H
#define __TASK_H

#include "basedef.h"

NS_FW_BASE_BEGIN

typedef enum TASK_STATE_ {
	TaskReady,
	TaskRunning,
	TaskCompleted,
	TaskInterrupted
} TASK_STATE;

class Task;

typedef DWORD (ACTION)(void*);

class Task {
	static DWORD threadProc(void* lpParameter);
	void initialize(ACTION* action, void* context);
protected: PROP(TASK_STATE, state);
protected: PROP(bool, isRunning);
protected: PROP_R(void*, context);
protected: PROP_R(ACTION*, action);
protected: HANDLE hThread_;
protected: DWORD threadId_;
public:
	Task();
	Task(ACTION* action);
	Task(ACTION* action, void* context, bool suspend = false);
	virtual ~Task();

	void action(ACTION*);

	Task* run();
	Task* reset();
	Task* stop();

	Task* wait(int ms = MAXINT32);
	DWORD result();
};

NS_FW_BASE_END

#endif