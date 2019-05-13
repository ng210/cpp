#ifndef __WINAPP_H
#define __WINAPP_H

#include "win/window.h"
#include "collection/map.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

class WinApp : public Window {
protected: PROP_R(char*, workingDir);
	//virtual HACCEL getAcceleratorTable();
protected:
	virtual void update();
public:
	WinApp() {};
	virtual ~WinApp();
	virtual int main(Map* args);

	HWND create(CREATESTRUCT* createStruct, WNDCLASSEX* wndClassEx);
};

extern WinApp* createApplication(HINSTANCE instance, Map* args);

#endif