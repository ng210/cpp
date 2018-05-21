#include "staticctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

StaticCtrl::StaticCtrl(Window* parent, size_t ctrlId, TCHAR* text) {
	CREATESTRUCT createStruct = {
		NULL,							// LPVOID      lpCreateParams;
		parent->hInstance(),			// HINSTANCE   hInstance;
		(HMENU)ctrlId,					// HMENU       hMenu;
		parent->hWnd(),					// HWND        hwndParent;
		CW_USEDEFAULT,					// int         cy;
		CW_USEDEFAULT,					// int         cx;
		CW_USEDEFAULT,					// int         y;
		CW_USEDEFAULT,					// int         x;
		WS_CHILD | WS_BORDER,			// LONG        style;
		text,							// LPCSTR      lpszName;
		"STATIC",						// LPCSTR      lpszClass;
		0								// DWORD       dwExStyle;
	};
	hWnd_ = create(&createStruct, parent);
}

StaticCtrl::~StaticCtrl() {
}

NS_FW_WIN_END