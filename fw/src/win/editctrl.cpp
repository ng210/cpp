#include "win/editctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

EditCtrl::EditCtrl(Window* parent, size_t ctrlId, int style, TCHAR* text) : Ctrl(parent, ctrlId) {
	if (style == 0) {
		style = WS_BORDER | ES_LEFT;
	}
	style |= WS_CHILD | WS_VISIBLE;

	CREATESTRUCT* createStruct = Ctrl::getCreateStruct(text, "EDIT");
	createStruct->style = style;
	hWnd_ = create(createStruct, parent);
	FREE(createStruct);
}

EditCtrl::~EditCtrl() {
}

NS_FW_WIN_END