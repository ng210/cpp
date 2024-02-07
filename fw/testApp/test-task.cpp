#include "testApp/testapp.h"
#include "test-task.h"

NS_FW_BASE_USE

static DWORD taskProc(TestTask*);
TestTask::TestTask(TestApp* app) : Task((ACTION*)& TestTask::taskProc) {
	app_ = app;
	ctrl_ = NEW_(StaticCtrl);
	ctrl_->create(app_, "Hello World");
	ctrl_->setSize(96, 24);
	ctrl_->move(100, 32);
	dx_ = 10;
	counter_ = 0;
}

TestTask::~TestTask() {
	stop();
	DEL_(ctrl_);
}

DWORD TestTask::taskProc(TestTask* task) {
	var c = task->ctrl_;
	var x = c->offset().x + task->dx_;
	var y = c->offset().y;
	RECT pr = c->parent()->rect();
	RECT cr = c->rect();
	if (x <= 0 || x >= pr.right - cr.right) {
		task->dx_ = -task->dx_;
		x += task->dx_;
		task->counter_++;
		y += 10;
	}
	c->move(x, y);
	RECT r1 = { cr.left + x, cr.top + y, cr.right + x, cr.bottom + y };
	SYSPR(InvalidateRect(c->parent()->hWnd(), &r1, false));
	Sleep(10);
	return (DWORD)(task->counter_ == 1);
}