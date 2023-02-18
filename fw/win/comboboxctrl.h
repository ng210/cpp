#ifndef __COMBOBOXCTRL_H
#define __COMBOBOXCTRL_H

#include "win/ctrl.h"
#include "collection/parray.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

class ComboboxCtrl : public Ctrl {
	static WndClass WndClass_;
protected: PROP_R(PArray*, items);
public:
	ComboboxCtrl();
	virtual ~ComboboxCtrl();

	void create(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);
	void addItem(LPTSTR item);
	void addItems(PArray* items);
	void setSelection(int ix);
	int getSelection();
};

NS_FW_WIN_END

#endif