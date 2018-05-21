#ifndef __EDITCTRL_H
#define __EDITCTRL_H

#include "win/ctrl.h"

NS_FW_WIN_BEGIN

class EditCtrl : public Ctrl {
public:
	EditCtrl(Window* parent, size_t ctrlId, int style = 0, TCHAR* text = NULL);
	~EditCtrl();
};

NS_FW_WIN_END

#endif