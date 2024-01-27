#ifndef __CTRL_H
#define __CTRL_H

#include "win/window.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

class Ctrl : public Window {
protected: PROP(size_t, ctrlId);
protected: PROP(POINT, offset);
protected:
public:
	Ctrl();

	void createWindow(Window* parent, char* name, LONG style = 0, DWORD exStyle = 0);

	void move(int x, int y);
};

NS_FW_WIN_END

#endif