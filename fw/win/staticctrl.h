#ifndef __STATICCTRL_H
#define __STATICCTRL_H

#include "win/ctrl.h"

NS_FW_WIN_BEGIN

class StaticCtrl : public Ctrl {
	static WndClass WndClass_;
public:
	WndClass getWindowClass();
};

NS_FW_WIN_END

#endif