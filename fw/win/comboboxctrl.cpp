#include "win/comboboxctrl.h"
#include <CommCtrl.h>

NS_FW_WIN_BEGIN

ComboboxCtrl::ComboboxCtrl(Window* parent, size_t ctrlId) : Ctrl(parent, ctrlId) {
	CREATESTRUCT* createStruct = Ctrl::getCreateStruct("bla", "ComboBox");	// WC_COMBOBOX);
	createStruct->style |= CBS_DROPDOWN | WS_VISIBLE | WS_VSCROLL;
	hWnd_ = create(createStruct, parent);
	FREE(createStruct);
	items_ = NEW_(PArray);
}

ComboboxCtrl::~ComboboxCtrl() {
	ARRAY_FOREACH(items_, FREE(value));
	DEL_(items_);
}

void ComboboxCtrl::addItem(LPTSTR item) {
	SendMessage(hWnd_, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)item);
}
void ComboboxCtrl::addItems(PArray* items) {
	ARRAY_FOREACH(items, SendMessage(hWnd_, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)value));
}

void ComboboxCtrl::setSelection(int ix) {
	SendMessage(hWnd_, CB_SETCURSEL, ix, 0);
}

int ComboboxCtrl::getSelection() {
	return (int)SendMessage(hWnd_, CB_GETCURSEL, 0, 0);
}

NS_FW_WIN_END