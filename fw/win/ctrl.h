#ifndef __CTRL_H
#define __CTRL_H

#include "win/window.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

class Ctrl : public Window {
protected: PROP_R(Window*, parent);
protected: PROP_R(size_t, ctrlId);
protected:
	Ctrl(Window* parent, size_t ctrlId);
	CREATESTRUCT* getCreateStruct(LPTSTR szName, LPTSTR szClass);
};

NS_FW_WIN_END

#endif