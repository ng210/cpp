#ifndef __BUTTONCTRL_H
#define __BUTTONCTRL_H

#include "win/ctrl.h"

NS_FW_WIN_BEGIN

class ButtonCtrl : public Ctrl {
	static WndClass WndClass_;
public:
	void create(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);
};

NS_FW_WIN_END

#endif