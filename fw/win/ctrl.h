#ifndef __CTRL_H
#define __CTRL_H

#include "win/window.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

class Ctrl : public Window {
protected: PROP(size_t, ctrlId);
protected:
	void create(WndClass wndClass, Window* parent, char* name, LONG style = 0, DWORD exStyle = 0);
public:
	Ctrl();
};

NS_FW_WIN_END

#endif