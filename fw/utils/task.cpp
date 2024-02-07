#include <windows.h>
#include "base/debug.h"
#include "utils/task.h"


NS_FW_BASE_USE

DWORD Task::threadProc(void* lpParameter) {
    var task = (Task*)lpParameter;
    DWORD exitCode = 0;
    while (task->state_ == TASK_STATE::TaskRunning) {
        exitCode = task->action_(task->context_);
        if (exitCode != 0) {
            break;
        }
        Sleep(1);
    }
    if (task->state_ == TASK_STATE::TaskRunning) {
        task->state_ = TASK_STATE::TaskCompleted;
    }
    task->isRunning_ = false;
    ExitThread(exitCode);
}

Task::Task() {
    action_ = NULL;
}

Task::Task(ACTION* action) {
    initialize(action, this);
}

Task::Task(ACTION* action, void* context, bool suspend) {
    if (context == NULL) {
        context = this;
        suspend = true;
    }
    initialize(action, context);
    if (!suspend) {
        run();
    }
}

Task::~Task() {
    if (isRunning_) {
        stop();
    }
    CloseHandle(hThread_);
}

void Task::initialize(ACTION* action, void* context) {
    action_ = action;
    context_ = context;
    hThread_ = NULL;
    threadId_ = 0;
    reset();
}

void Task::action(ACTION* a) {
    if (action_ == NULL) {
        initialize(a, context_);
    }
    else {
        stop();
        action_ = a;
        // start(); ??
    }
}

Task* Task::run() {
    if (hThread_ != NULL) {
        SYSPR(ResumeThread(hThread_));
        state_ = TASK_STATE::TaskRunning;
        isRunning_ = true;
    }
    return this;
}

Task* Task::reset() {
    if (hThread_ != NULL) {
        CloseHandle(hThread_);
    }
    isRunning_ = false;
    hThread_ = CreateThread(0, 0x1000, &Task::threadProc, this, CREATE_SUSPENDED, &threadId_);
    state_ = TASK_STATE::TaskReady;
    return this;
}

Task* Task::stop() {
    state_ = TASK_STATE::TaskInterrupted;
    // wait for thread to finish
    while (isRunning_) {
        Sleep(10);
    }
    return this;
}

Task* Task::wait(int ms) {
    int ti = 0;
    while (ti < ms) {
        if (state_ != TASK_STATE::TaskRunning) {
            break;
        }
        Sleep(10);
        ti++;
    }
    return this;
}

DWORD Task::result() {
    DWORD exitCode = 0;
    GetExitCodeThread(hThread_, &exitCode);
    return exitCode;
}