#ifndef __EDITCTRL_H
#define __EDITCTRL_H

#include "win/ctrl.h"

NS_FW_WIN_BEGIN

class EditCtrl : public Ctrl {
	static char* windowClassName_;
public:
	EditCtrl();
	~EditCtrl();

	char* const registerWindowClass();
	void createWindow(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);

	void setText(TCHAR* text);
};

NS_FW_WIN_END

#endif