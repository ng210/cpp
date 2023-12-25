#include "player/src/device-ctrl.h"

using namespace PLAYER;

WndClass DeviceCtrl::WndClass_;

DeviceCtrl::DeviceCtrl(DeviceExt* deviceExt) : deviceExt_(deviceExt) {
	if (DeviceCtrl::wndClass_.atom == 0) {
		DeviceCtrl::wndClass_.atom = registerClass("DeviceCtrl", NULL, NULL);
	}
}

DeviceCtrl::~DeviceCtrl() {
}

WndClass DeviceCtrl::getWindowClass() {
	return DeviceCtrl::WndClass_;
}

void DeviceCtrl::createWindow(Window* parent, char* name, LONG style, DWORD exStyle) {
	Ctrl::createWindow(parent, name, style, exStyle);
}

void DeviceCtrl::onSelectProgram(ComboboxCtrl* cb, int ix, void* item) {
	var ctrl = (DeviceCtrl*)cb->parent();
	DeviceExt::presetSetter(ctrl->deviceExt_, ix);
}

LRESULT DeviceCtrl::onAddProgram(Window* button, POINT& pos, WPARAM state) {
	var devCtrl = (DeviceCtrl*)button->parent();
	char name[16];
	devCtrl->presetCtrl_.getText(name, 16);
	var pb = devCtrl->deviceExt_->presetBank();
	var ix = pb->indexOf(name);
	if (ix == -1) {
		// add new program
		var prg = MALLOC(byte, pb->bankSize());
		devCtrl->deviceExt_->writePreset(prg);
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

LRESULT DeviceCtrl::onRemoveProgram(Window* button, POINT& pos, WPARAM state) {
	var mdlCtrl = (ModuleCtrl*)button->parent();
	var mdl = mdlCtrl->module_;
	char name[16];
	mdlCtrl->programCtrl_.getText(name, 16);
	var sb = mdl->soundbank();
	if (sb->size() > 1) {
		var ix = sb->indexOf(name);
		if (ix != -1) {
			if (mdl->program() == ix) {
				Module::programSetter(mdl, ix > 0 ? 0 : 1);
			}
			// remove program
			Key key(&name);
			sb->remove(key);
			mdlCtrl->updateSoundbank();
		}
	}
	return 1;
}



int DeviceCtrl::presetBankSetter(void* obj, PresetBank* pb, void* unused) {
	var ctrl = (DeviceCtrl*)obj;
	ctrl->deviceExt_->setPresetBank(pb);
	//ctrl->updateSoundbank();
	return 1;
}
int DeviceCtrl::presetSetter(void* obj, int ix, void* unused) {
	var ctrl = (DeviceCtrl*)obj;
	LRESULT res = 0;
	SYSPR2(SendMessage(ctrl->presetCtrl_.hWnd(), CB_GETLBTEXT, res, ix), LB_ERR);
	ctrl->presetCtrl_.select(ix);
	return 1;
}

