#ifndef __BUTTONCTRL_H
#define __BUTTONCTRL_H

#include "win/ctrl.h"

NS_FW_WIN_BEGIN

class ButtonCtrl : public Ctrl {
	static WndClass WndClass_;
public:
	WndClass getWindowClass();
};

NS_FW_WIN_END

#endif