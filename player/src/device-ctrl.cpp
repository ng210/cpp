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
	//TODO: DeviceExt::presetBankSetter(ctrl->deviceExt_, ix, NULL);
}

LRESULT DeviceCtrl::onAddProgram(Window* button, POINT& pos, WPARAM state) {
	var devCtrl = (DeviceCtrl*)button->parent();
	var device = (Device*)devCtrl->deviceExt_;
	char name[16];
	devCtrl->presetCtrl_.getText(name, 16);
	var pb = device->presetBank();
	var ix = pb->indexOf(name);
	if (ix == -1) {
		// add new preset
		var prg = NEW_(Stream, pb->bankSize());
		devCtrl->deviceExt_->writePresetToStream(prg);
		pb->add(name, prg->data());
		DEL_(prg);
		//devCtrl->updateSoundbank();
	} else {
		var prg = NEW_(Stream, (byte*)pb->get(&name[0]), pb->bankSize());
		devCtrl->deviceExt_->writePresetToStream(prg);
	}
	SYSFN2(ix, (int)SendMessage(devCtrl->presetCtrl_.hWnd(), CB_FINDSTRINGEXACT, 0, (LPARAM)name), CB_ERR);
	device->setPreset(ix);
	return 1;
}

LRESULT DeviceCtrl::onRemoveProgram(Window* button, POINT& pos, WPARAM state) {
	var devCtrl = (DeviceCtrl*)button->parent();
	var dev = (Device*)devCtrl->deviceExt_;
	char name[16];
	devCtrl->presetCtrl_.getText(name, 16);
	var sb = dev->presetBank();
	if (sb->size() > 1) {
		var ix = sb->indexOf(name);
		if (ix != -1) {
			if (dev->preset() == ix) {
				//TODO: DeviceExt::presetSetter(dev, ix > 0 ? 0 : 1, NULL);
			}
			// remove preset
			Key key(&name);
			sb->remove(key);
			//devCtrl->updateSoundbank();
		}
	}
	return 1;
}

int DeviceCtrl::presetBankSetter(void* obj, PresetBank* pb, void* unused) {
	var ctrl = (DeviceCtrl*)obj;
	((Device*)ctrl->deviceExt_)->setPresetBank(pb);
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

