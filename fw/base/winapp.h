#ifndef __WINAPP_H
#define __WINAPP_H

#include "base/windowwrapper.h"
#include "collection/map.h"

NS_FW_BASE_USE

extern int winMain(HINSTANCE instance, Map* args);

//#ifdef UNICODE
//#define WinMain WinMainW
//#else
//#define WinMain WinMainA
//#endif

extern int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow);
extern int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow);

NS_FW_BASE_BEGIN

class WinApp : public WindowWrapper {
private:
	static ATOM windowClass_;
protected:
	virtual HACCEL getAcceleratorTable();
public:
	~WinApp();
	int main(LPCWSTR cmdLine);
};

NS_FW_BASE_END

#endif