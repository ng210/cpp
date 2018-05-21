#include "editctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

EditCtrl::EditCtrl(Window* parent, size_t ctrlId, int style, TCHAR* text) {
	if (style == 0) {
		style = WS_BORDER | ES_LEFT;
	}
	style |= WS_CHILD | WS_VISIBLE;
	CREATESTRUCT createStruct = {
		NULL,							// LPVOID      lpCreateParams;
		parent->hInstance(),			// HINSTANCE   hInstance;
		(HMENU)ctrlId,					// HMENU       hMenu;
		parent->hWnd(),					// HWND        hwndParent;
		CW_USEDEFAULT,					// int         cy;
		CW_USEDEFAULT,					// int         cx;
		CW_USEDEFAULT,					// int         y;
		CW_USEDEFAULT,					// int         x;
		style,							// LONG        style;
		text,							// LPCSTR      lpszName;
		"EDIT",							// LPCSTR      lpszClass;
		0								// DWORD       dwExStyle;
	};
	hWnd_ = create(&createStruct, parent);
}

EditCtrl::~EditCtrl() {
}

NS_FW_WIN_END