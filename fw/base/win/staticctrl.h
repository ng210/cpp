#ifndef __STATICCTRL_H
#define __STATICCTRL_H

#include "win/ctrl.h"

NS_FW_WIN_BEGIN

class StaticCtrl : public Ctrl {
public:
	StaticCtrl(Window* parent, size_t ctrlId, TCHAR* text = NULL);
	~StaticCtrl();
};

NS_FW_WIN_END

#endif