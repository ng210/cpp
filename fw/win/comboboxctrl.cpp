#include "win/comboboxctrl.h"
//#include <CommCtrl.h>

NS_FW_WIN_BEGIN

char* ComboboxCtrl::windowClassName_ = "COMBOBOX";

ComboboxCtrl::ComboboxCtrl() {
	edit_ = NULL;
	list_ = NULL;
	onAddItem_ = &ComboboxCtrl::defAddItemProc_;
	onSelectItem_ = &ComboboxCtrl::defSelectItemProc_;
}

WNDPROC ComboboxCtrl::defEditWndProc_ = NULL;

ComboboxCtrl::~ComboboxCtrl() {
}

char* const ComboboxCtrl::registerWindowClass() {
	return ComboboxCtrl::windowClassName_;
}

LRESULT ComboboxCtrl::editWndProc_(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT res = 0;
	char item[32];
	var cb = (ComboboxCtrl*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (uMsg) {
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) {
			SYSPR(GetWindowText(hWnd, item, 32));
			SYSFN2(res, SendMessage(cb->hWnd(), CB_FINDSTRINGEXACT, -1, (LPARAM)item), CB_ERR);
			if (res != CB_ERR) {
				cb->onSelectItem()(cb, (int)res, item);
			}
			else {
				cb->onAddItem()(cb, item);
			}
		}
		break;
	}
	return cb->defEditWndProc_(hWnd, uMsg, wParam, lParam);
}

BOOL enumChildren(HWND hWnd, LPARAM lParam) {
	((Array*)lParam)->push(&hWnd);
	return false;
}

void ComboboxCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	if (!(style & CBS_SIMPLE) && !(style & CBS_DROPDOWN) && !(style & CBS_DROPDOWNLIST)) {
		style |= CBS_DROPDOWNLIST;
	}
	Ctrl::create(parent, name, style, exStyle);
	Array children(sizeof(HWND));
	SYSPR(EnumChildWindows(hWnd_, enumChildren, (LPARAM)&children));
	
	// get listbox item
	var hFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	char className[20];
	for (var i = 0; i < children.length(); i++) {
		var hWnd = *(HWND*)children.get(i);
		SYSPR(GetClassName(hWnd, className, 20));
		if (fmw::strncmp(className, "Edit", 20) == 0) {
			edit_ = hWnd;
			SYSPR(SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this));
			var SYSFN(long_ptr, SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)ComboboxCtrl::editWndProc_));
			defEditWndProc_ = (WNDPROC)long_ptr;
			SYSPR(SendMessage(edit_, WM_SETFONT, (WPARAM)hFont, true));
		}
		else {
			list_ = hWnd;
			SYSPR(SendMessage(list_, WM_SETFONT, (WPARAM)hFont, true));
		}
	}	
}

LRESULT CALLBACK ComboboxCtrl::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT res = 0;
	char item[32];
	//NMHDR nmhdr = { hWnd_, 0, CBN_ITEMSELECTED };
	switch (uMsg) {
	case WM_COMMAND:
		word noteId, ctrlId;
		noteId = HIWORD(wParam);
		ctrlId = LOWORD(wParam);
		//LOG("%x %x\n", ctrlId, noteId);
		switch (noteId) {
		case LBN_SELCHANGE:
			SYSFN2(res, SendMessage(hWnd_, CB_GETCURSEL, NULL, NULL), LB_ERR);
			SYSPR2(SendMessage(hWnd_, CB_GETLBTEXT, res, (LPARAM)item), LB_ERR);
			onSelectItem()(this, (int)res, item);
			break;
		case EN_CHANGE:
			int len;
			len = fmw::strlen(item);
			SYSPR(GetWindowText(hWnd_, item, 32));
			SYSFN(res, SendMessage(hWnd_, CB_FINDSTRING, -1, (LPARAM)item));
			if (res == CB_ERR) {
				SendMessage(list_, LB_SETCURSEL, -1, NULL);
				//nmhdr.code = CBN_NEWITEM;
				//SYSPR(SendMessage(parent_->hWnd(), WM_NOTIFY, (WPARAM)item, (LPARAM)&nmhdr));
			}
			else {
				SYSPR(SendMessage(hWnd_, CB_SETCURSEL, res, NULL));
				SYSPR(SendMessage(hWnd_, CB_SETEDITSEL, NULL, 0xffff0000 | len));
			}
			break;
		}
		res = Ctrl::wndProc(hWnd, uMsg, wParam, lParam);
		//if (ctrlId == editId_ && noteId == EN_CHANGE) {
		//	int len;
		//	len = fmw::strlen(item);
		//	SYSPR(GetWindowText(hWnd_, item, 32));
		//	SYSFN(res, SendMessage(hWnd_, CB_FINDSTRING, -1, (LPARAM)item));
		//	if (res == CB_ERR) {
		//		SendDlgItemMessage(hWnd_, listBoxId_, LB_SETCURSEL, -1, NULL);
		//		//nmhdr.code = CBN_NEWITEM;
		//		//SYSPR(SendMessage(parent_->hWnd(), WM_NOTIFY, (WPARAM)item, (LPARAM)&nmhdr));
		//	}
		//	else {
		//		SYSPR(SendMessage(hWnd_, CB_SETCURSEL, res, NULL));
		//		SYSPR(SendMessage(hWnd_, CB_SETEDITSEL, NULL, 0xffff0000 | len));
		//	}
		//}
		//else {
		//	if (ctrlId == listBoxId_) {
		//		SYSFN2(res, SendDlgItemMessage(hWnd_, listBoxId_, LB_GETCURSEL, NULL, NULL), LB_ERR);
		//		SYSPR2(SendDlgItemMessage(hWnd_, listBoxId_, LB_GETTEXT, res, (LPARAM)item), LB_ERR);
		//		onSelectItem()(this, (int)res, item);
		//	}
		//	res = defWindowProc_(hWnd, uMsg, wParam, lParam);
		//}
		//if (ctrlId == listBoxId_ && noteId == LBN_SELCHANGE) {
		//	SYSFN(res, SendMessage(hWnd_, CB_SETCURSEL, res, NULL));
		//	SYSPR(SendMessage(hWnd_, WM_SETTEXT, NULL, (LPARAM)item));
		//}
		break;
	default:
		res = Ctrl::wndProc(hWnd, uMsg, wParam, lParam);
	}
	return res;
}

LRESULT ComboboxCtrl::handleNotification(int notification, LPARAM lParam) {
	//char item[32];
	//LRESULT res;
	//NMHDR nmhdr = { hWnd_, 0, 0 };
	//GetWindowText(hWnd_, item, 32);
	//switch (notification) {
	//case CBN_EDITUPDATE:
	//	SYSFN(res, SendMessage(hWnd_, CB_FINDSTRING, -1, (LPARAM)item));
	//	if (res != CB_ERR) {
	//		int len;
	//		len = fmw::strlen(item);
	//		SYSFN(res, SendMessage(hWnd_, CB_SETCURSEL, res, NULL));
	//		SYSFN(res, SendMessage(hWnd_, CB_SETEDITSEL, NULL, 0xffff0000 | len));
	//	}
	//	break;
	//case CBN_SELENDOK:
	//	if (item[0] != '\0') {
	//		SYSFN(res, SendMessage(hWnd_, CB_GETCURSEL, NULL, NULL));
	//		if (res == CB_ERR) {
	//			nmhdr.idFrom = items_.length();
	//			nmhdr.code = CBN_NEWITEM;
	//		}
	//		else {
	//			SYSFN(nmhdr.idFrom, SendMessage(hWnd_, CB_GETITEMDATA, res, NULL));
	//			nmhdr.code = CBN_ITEMSELECTED;
	//		}
	//		SYSPR(SendMessage(parent_->hWnd(), WM_NOTIFY, (WPARAM)&item, (LPARAM)&nmhdr));
	//	}
	//	break;
	//}
	return 0;
}

void ComboboxCtrl::addItem(void* item) {
	var ix = items_.length();
	items_.push(item);
	SYSPR(SendMessage(hWnd_, CB_ADDSTRING, NULL, (LPARAM)item));
	var SYSFN(res, SendMessage(hWnd_, CB_FINDSTRING, NULL, (LPARAM)item));
	SYSPR(SendMessage(hWnd_, CB_SETITEMDATA, res, ix));
}
void ComboboxCtrl::addItems(PArray* items) {
	var ix = items_.length();
	for (var i = 0; i < items->length(); i++) {
		var item = items->get(i);
		items_.push(item);
		SYSPR(SendMessage(hWnd_, CB_ADDSTRING, NULL, (LPARAM)item));
		SYSPR(SendMessage(hWnd_, CB_SETITEMDATA, ix, ix));
		ix++;
	}
}

LRESULT ComboboxCtrl::getText(char* text, int count) {
	var SYSFN(result, SendMessage(edit_, WM_GETTEXT, count, (LPARAM)text));
	return result;
}

void ComboboxCtrl::select(int ix) {
	SendMessage(hWnd_, CB_SETCURSEL, ix, 0);
}

void ComboboxCtrl::select(void* item) {
	var SYSFN(ix, SendMessage(hWnd_, CB_FINDSTRING, NULL, (LPARAM)item));
	SendMessage(hWnd_, CB_SETCURSEL, ix, 0);
}

int ComboboxCtrl::select() {
	return (int)SendMessage(hWnd_, CB_GETCURSEL, 0, 0);
}

void ComboboxCtrl::reset() {
	SYSPR(SendMessage(hWnd_, CB_RESETCONTENT, NULL, NULL));
}

void ComboboxCtrl::defAddItemProc_(ComboboxCtrl* cb, void* item) {
	cb->addItem(item);
}
void ComboboxCtrl::defSelectItemProc_(ComboboxCtrl* cb, int ix, void* item) {

}

NS_FW_WIN_END