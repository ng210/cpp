#include "synth/ui/module-ctrl.h"

using namespace SYNTH_UI;

WndClass ModuleCtrl::wndClass_;

ModuleCtrl::ModuleCtrl(Module* module) {
	if (ModuleCtrl::wndClass_.atom == 0) {
		ModuleCtrl::wndClass_.atom = registerClass("ModuleCtrl", NULL, CS_OWNDC);
	}
	module_ = module;
	potCtrlCount_ = 0;
	potCtrls_ = NULL;
	potPosition_ = NULL;
}

ModuleCtrl::~ModuleCtrl() {
	for (var i = 0; i < potCtrlCount_; i++) {
		DEL_(potCtrls_[i]);
	}
	FREE(potCtrls_);
	FREE(potPosition_);
}

void ModuleCtrl::create(Window* parent, char* name) {
	Ctrl::create(ModuleCtrl::wndClass_, parent, name);
}

void ModuleCtrl::initFromStream(Stream* data) {
	int x = 0, y = 0;
	potCtrlCount_ = data->readByte();
	potCtrls_ = MALLOC(PotCtrl*, potCtrlCount_);
	potPosition_ = MALLOC(POINT, potCtrlCount_);
	for (var i = 0; i < potCtrlCount_; i++) {
		var ctrlId = data->readByte();						// ctrl id
		potPosition_[i].x = data->readWord();				// x
		potPosition_[i].y = data->readWord();				// y
		char* label = data->readString();					// label
		var ctrl = module_->getControl(ctrlId);
		potCtrls_[i] = NEW_(PotCtrl);
		potCtrls_[i]->create(this, label);
		potCtrls_[i]->pot(ctrl);
		potCtrls_[i]->type((PotCtrlType)data->readByte());	// type
		potCtrls_[i]->setSize(128);
		SetWindowPos(potCtrls_[i]->hWnd(), NULL, potPosition_[i].x, potPosition_[i].y, 0, 0, SWP_NOSIZE);
		var cx = potPosition_[i].x + potCtrls_[i]->rect().right;
		var cy = potPosition_[i].y + potCtrls_[i]->rect().bottom;
		if (cx > x) x = cx;
		if (cy > y) y = cy;
	}

	SetWindowPos(hWnd_, NULL, 0, 0, x, y, SWP_NOMOVE);
}

LRESULT ModuleCtrl::onPaint() {
	PAINTSTRUCT ps;
	var hdc = BeginPaint(hWnd_, &ps);
	FillRect(hdc, &rect_, PotCtrl::foregroundBrush_);
	EndPaint(hWnd_, &ps);
	return 0;
}