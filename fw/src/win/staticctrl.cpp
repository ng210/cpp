#include "win/staticctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

StaticCtrl::StaticCtrl(Window* parent, size_t ctrlId, TCHAR* text) : Ctrl(parent, ctrlId) {
	CREATESTRUCT* createStruct = Ctrl::getCreateStruct(text, "STATIC");
	hWnd_ = create(createStruct, parent);
	FREE(createStruct);
}

StaticCtrl::~StaticCtrl() {
}

NS_FW_WIN_END