#ifndef __COMBOBOXCTRL_H
#define __COMBOBOXCTRL_H

#include "win/ctrl.h"
#include "collection/parray.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

#define CBN_ITEMSELECTED 98
#define CBN_NEWITEM 99

class ComboboxCtrl;
typedef void (CBADDITEMPROC)(ComboboxCtrl*, void*);
typedef void (CBSELECTITEMPROC)(ComboboxCtrl*, int, void*);

class ComboboxCtrl : public Ctrl {
	static char* windowClassName_;
	static LRESULT editWndProc_(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static WNDPROC defEditWndProc_;
	static CBADDITEMPROC defAddItemProc_;
	static CBSELECTITEMPROC defSelectItemProc_;
protected: PROP_R(PArray, items);
protected: PROP_R(HWND, list);
protected: PROP_R(HWND, edit);
protected: PROP(CBADDITEMPROC*, onAddItem);
protected: PROP(CBSELECTITEMPROC*, onSelectItem);
public:
	ComboboxCtrl();
	~ComboboxCtrl();

	char* const registerWindowClass();
	void create(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);
	LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT handleNotification(int notification, LPARAM lParam);

	void addItem(void* item);
	void addItems(PArray* items);
	LRESULT getText(char* text, int count = 20);
	void select(int ix);
	void select(void* item);
	int select();
	void reset();
};

NS_FW_WIN_END

#endif