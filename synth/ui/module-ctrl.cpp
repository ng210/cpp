#include "synth/ui/module-ctrl.h"

using namespace SYNTH_UI;

WndClass ModuleCtrl::wndClass_;

HANDLE ModuleCtrl::hBackground_ = NULL;

ModuleCtrl::ModuleCtrl(Module* module) {
	if (ModuleCtrl::wndClass_.atom == 0) {
		ModuleCtrl::wndClass_.atom = registerClass("ModuleCtrl", NULL, CS_OWNDC);
		hBackground_ = NULL;
	}
	module_ = module;
	//module_->setControl = { this, &ModuleCtrl::setControlProc };
	//module_->setSoundbank = { this, &ModuleCtrl::setSoundbankProc };
	//module_->setProgram = { this, &ModuleCtrl::setProgramProc };
	potCtrlCount_ = 0;
	potCtrls_ = NULL;
	borderWidth_ = 4;
	gapWidth_ = 1;

	module_->setSoundbank.add(this, &ModuleCtrl::soundbankSetter);
	module_->setProgram.add(this, &ModuleCtrl::programSetter);
}

ModuleCtrl::~ModuleCtrl() {
	for (var i = 0; i < potCtrlCount_; i++) {
		DEL_(potCtrls_[i]);
	}
	FREE(potCtrls_);
}

void ModuleCtrl::create(Window* parent, char* name) {
	Ctrl::create(ModuleCtrl::wndClass_, parent, name);
	RECT rect = { borderWidth_, borderWidth_, 144, 96 };
	if (module_->soundbank() != NULL) {
		// add combobox for soundbank
		programCtrl_.create(this, "Program", CBS_DROPDOWN | WS_VSCROLL);
		programCtrl_.onAddItem([](ComboboxCtrl* cb, void* item){});
		programCtrl_.onSelectItem([](ComboboxCtrl* cb, int ix, void* item){});
		SYSPR(SetWindowPos(programCtrl_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
		programCtrl_.onSelectItem(ModuleCtrl::onSelectProgram);
		rect.left += rect.right + 2;
		updateSoundbank();
		// add buttons add/del
		rect.right = 24; rect.bottom = 24;
		addButton_.create(this, "Add Prg", BS_CENTER | BS_TEXT);
		SYSPR(SetWindowText(addButton_.hWnd(), "+"));
		SYSPR(SetWindowPos(addButton_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
		rect.left += rect.right + 2;
		addButton_.onLeftUp(ModuleCtrl::onAddProgram);
		removeButton_.create(this, "Del Prg", BS_CENTER | BS_TEXT);
		SYSPR(SetWindowText(removeButton_.hWnd(), "-"));
		SYSPR(SetWindowPos(removeButton_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
		removeButton_.onLeftUp(ModuleCtrl::onRemoveProgram);
		programCtrl_.select(module_->program());
	}
	var hBackground = getBackgroundImage();
	background_.create(this, "bgbitmap", WS_CLIPSIBLINGS | SS_BITMAP | SS_SUNKEN);
	SYSPR(SendMessage(background_.hWnd(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBackground));
	SYSPR(SetWindowPos(background_.hWnd(), NULL, 0, 0, rect_.right, rect_.bottom, SWP_SHOWWINDOW));
}

void ModuleCtrl::initFromStream(Stream* data, int size) {
	int x = borderWidth_, y = borderWidth_ + 24 + borderWidth_;
	int width = 0, height = 0;
	potCtrls_ = MALLOC(PotCtrl*, 256);
	var pCtrl = potCtrls_;
	potCtrlCount_ = 0;

	int ctrlId;
	while ((ctrlId = data->readByte()) != LayoutEnd) {
		if (ctrlId == LayoutHorizontalGap) {
			x += 2*gapWidth_;
			continue;
		} else if (ctrlId == LayoutVerticalGap) {
			x = borderWidth_;
			y = height + gapWidth_;
			continue;
		}

		char* label = data->readString();
		var ctrl = module_->getControl(ctrlId);
		*pCtrl = NEW_(PotCtrl);
		potCtrlCount_++;
		(*pCtrl)->create(this, label);
		(*pCtrl)->pot(ctrl);
		(*pCtrl)->type((PotCtrlType)data->readByte());
		(*pCtrl)->setSize(size);
		SetWindowPos((*pCtrl)->hWnd(), NULL, x, y, 0, 0, SWP_NOSIZE);
		x += (*pCtrl)->rect().right + gapWidth_;
		var cy = y + (*pCtrl)->rect().bottom;
		if (width < x) width = x;
		if (height < cy) height = cy;
		pCtrl++;
	}

	SetWindowPos(hWnd_, NULL, 0, 0, width + borderWidth_, height + borderWidth_, SWP_NOMOVE);
}

HANDLE ModuleCtrl::getBackgroundImage() {
	if (hBackground_ == NULL) {
		SYSFN(hBackground_, LoadImage(NULL, "mdl-default-bg.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));
	}
	return hBackground_;
}

//LRESULT ModuleCtrl::onPaint() {
//	PAINTSTRUCT ps;
//	var hdc = BeginPaint(hWnd_, &ps);
//	FillRect(hdc, &rect_, PotCtrl::foregroundBrush_);
//	// program select
//
//	EndPaint(hWnd_, &ps);
//	return 0;
//}

LRESULT ModuleCtrl::onSize(RECT& rect, WPARAM state) {
	SYSPR(SetWindowPos(background_.hWnd(), HWND_BOTTOM, 0, 0, rect.right, rect.bottom, SWP_SHOWWINDOW));
	return 1;
}


void ModuleCtrl::updateSoundbank() {
	if (module_ != NULL) {
		programCtrl_.reset();
		var sb = module_->soundbank();
		for (var i = 0; i < (int)sb->size(); i++) {
			var prgName = sb->keys()->get(i);
			programCtrl_.addItem(prgName);
		}
	}
}

//void ModuleCtrl::updateProgram() {
//	if (module_ != NULL) {
//		SYSPR(SendMessage(programCtrl_.hWnd(), CB_SETCURSEL, module_->program(), NULL));
//	}
//}

void ModuleCtrl::onSelectProgram(ComboboxCtrl* cb, int ix, void* item) {
	var ctrl = (ModuleCtrl*)cb->parent();
	Module::programSetter(ctrl->module_, ix);
}

LRESULT ModuleCtrl::onAddProgram(Window* button, POINT& pos, WPARAM state) {
	var mdlCtrl = (ModuleCtrl*)button->parent();
	char name[16];
	mdlCtrl->programCtrl_.getText(name, 16);
	var sb = mdlCtrl->module()->soundbank();
	var ix = sb->indexOf(name);
	if (ix == -1) {
		// add new program
		var prg = MALLOC(byte, sb->bankSize());
		mdlCtrl->module()->writeProgram(prg);
		sb->add(name, prg);
		FREE(prg);
		mdlCtrl->updateSoundbank();
	}
	else {
		var prg = (byte*)sb->get(&name[0]);
		mdlCtrl->module()->writeProgram(prg);
	}
	SYSFN2(ix, (int)SendMessage(mdlCtrl->programCtrl_.hWnd(), CB_FINDSTRINGEXACT, 0, (LPARAM)name), CB_ERR);
	mdlCtrl->module()->setProgram(ix);
	return 1;
}

LRESULT ModuleCtrl::onRemoveProgram(Window* button, POINT& pos, WPARAM state) {
	var mdlCtrl = (ModuleCtrl*)button->parent();
	char name[16];
	mdlCtrl->programCtrl_.getText(name, 16);
	var sb = mdlCtrl->module()->soundbank();
	var ix = sb->indexOf(name);
	if (ix != -1) {
		// remove program
		sb->remove(ix);
		mdlCtrl->updateSoundbank();
	}
	return 1;
}

PotCtrl* ModuleCtrl::getControl(int id) {
	return id < potCtrlCount_ ? potCtrls_[id] : NULL;
}

int ModuleCtrl::soundbankSetter(void* obj, Soundbank* sb) {
	var ctrl = (ModuleCtrl*)obj;
	//Module::soundbankSetter(ctrl->module_, sb);
	ctrl->updateSoundbank();
	return 1;
}
int ModuleCtrl::programSetter(void* obj, int ix) {
	var ctrl = (ModuleCtrl*)obj;
	LRESULT res = 0;
	SYSPR2(SendMessage(ctrl->programCtrl_.hWnd(), CB_GETLBTEXT, res, ix), LB_ERR);
	//var SYSFN(item, SendMessage(ctrl->programCtrl_.hWnd(), CB_GETLBTEXT, NULL, ix));
	ctrl->programCtrl_.select(ix);
	//Module::programSetter(ctrl->module_, ix);
	return 1;
}

//void ModuleCtrl::setControlProc(void* mdl, int id, S value) {
//	var mdlCtrl = (ModuleCtrl*)mdl;
//	Module::setControlProc(mdlCtrl->module_, id, value);
//}

//void ModuleCtrl::setSoundbankProc(void* mdl, byte* sb) {
//	var mdlCtrl = (ModuleCtrl*)mdl;
//	Module::setSoundbankProc(mdlCtrl->module_, sb);
//	mdlCtrl->updateSoundbank();
//}
//void ModuleCtrl::setProgramProc(void* mdl, int id) {
//	var mdlCtrl = (ModuleCtrl*)mdl;
//	Module::setProgramProc(mdlCtrl->module_, id);
//	mdlCtrl->updateProgram();
//}
//
//void ModuleCtrl::addItemProc(ComboboxCtrl* cb, void* item) {
//	var mdlCtrl = (ModuleCtrl*)cb->parent();
//	// collect settings
//	Stream preset(256);
//	var mdl = mdlCtrl->module();
//	// store in sound bank
//	mdl->addPreset((char*)name, preset);
//	cb->
//}
//void ModuleCtrl::selectItemProc(ComboboxCtrl* cb, int ix, void* item) {
//	var mdlCtrl = (ModuleCtrl*)cb->parent();
//}