#ifndef __CTRL_H
#define __CTRL_H

#include "win/window.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

class Ctrl : public Window {
protected:
	Ctrl();
public:
	virtual ~Ctrl();
};

NS_FW_WIN_END

#endif