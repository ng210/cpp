#ifndef __COMBOBOXCTRL_H
#define __COMBOBOXCTRL_H

#include "win/ctrl.h"
#include "collection/parray.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

class ComboboxCtrl : public Ctrl {
protected: PROP_R(PArray*, items);
public:
	ComboboxCtrl(Window* parent, size_t ctrlId);
	virtual ~ComboboxCtrl();
	void addItem(LPTSTR item);
	void addItems(PArray* items);
	void setSelection(int ix);
	int getSelection();
};

NS_FW_WIN_END

#endif