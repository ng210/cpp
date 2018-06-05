#include "Ctrl.h"

NS_FW_WIN_BEGIN

Ctrl::Ctrl(Window* parent, size_t ctrlId) {
	parent_ = parent;
	ctrlId_ = ctrlId;
}

CREATESTRUCT* Ctrl::getCreateStruct(LPTSTR szName, LPTSTR szClass) {
	CREATESTRUCT* createStruct = MALLOC(CREATESTRUCT, 1);
	createStruct->lpCreateParams = NULL;
	createStruct->hInstance = parent_->hInstance();
	createStruct->hMenu = (HMENU)ctrlId_;
	createStruct->hwndParent = parent_->hWnd();
	createStruct->cx = CW_USEDEFAULT;
	createStruct->cy = CW_USEDEFAULT;
	createStruct->x = CW_USEDEFAULT;
	createStruct->y = CW_USEDEFAULT;
	createStruct->style = WS_CHILD;
	createStruct->lpszName = szName;
	createStruct->lpszClass = szClass;
	createStruct->dwExStyle = 0;
	return createStruct;
};

NS_FW_WIN_END