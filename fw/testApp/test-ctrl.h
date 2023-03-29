#ifndef __TEST_CTRL_H
#define __TEST_CTRL_H

#include "win\ctrl.h"

NS_FW_WIN_USE

class TestCtrl : public Ctrl {
	char* text_;
	static WndClass WndClass_;
	HBRUSH hBrush_;
public:
	TestCtrl();
	~TestCtrl();

	void create(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);

	void setText(char* text);

	LRESULT onCreate();
	LRESULT onPaint();

	static MOUSEEVENTPROC onLeftUpProc;
};

#endif