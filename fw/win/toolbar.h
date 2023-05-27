#ifndef __TOOLBAR_H
#define __TOOLBAR_H

#include "win/ctrl.h"

NS_FW_WIN_BEGIN

class Toolbar : public Ctrl {
	static WndClass WndClass_;
public:
	Toolbar();
	~Toolbar();

	void create(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);
};

NS_FW_WIN_END

#endif