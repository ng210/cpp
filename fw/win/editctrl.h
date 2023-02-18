#ifndef __EDITCTRL_H
#define __EDITCTRL_H

#include "win/ctrl.h"

NS_FW_WIN_BEGIN

class EditCtrl : public Ctrl {
	static WndClass WndClass_;
public:
	EditCtrl();
	~EditCtrl();

	void create(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);
};

NS_FW_WIN_END

#endif