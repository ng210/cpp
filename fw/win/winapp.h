#ifndef __WINAPP_H
#define __WINAPP_H

#include "win/window.h"
#include "collection/map.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

class WinApp : public Window {
protected: PROP_R(char*, workingDir);
//virtual HACCEL getAcceleratorTable();

protected:
	virtual void update();
public:
	WinApp();
	virtual ~WinApp();
	virtual int main(Map* args);
};

NS_FW_WIN_END

NS_FW_WIN_USE
extern WinApp* createApplication(HINSTANCE instance, Map* args);

#endif