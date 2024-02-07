#include "player/src/device-ctrl.h"

using namespace PLAYER;

#define CHARWIDTH 10
#define CHARHEIGHT 24

char* DeviceCtrl::windowClassName_ = "DeviceCtrl";
ATOM DeviceCtrl::windowClass_;

DeviceCtrl::DeviceCtrl(DeviceExt* deviceExt) {
	deviceExt_ = deviceExt;
	inputControls_ = NULL;
	borderWidth_ = 2;
	hBackgroundImage_ = NULL;
}

DeviceCtrl::~DeviceCtrl() {
	DELARR(inputControls_);
}

char* const DeviceCtrl::registerWindowClass() {
	if (DeviceCtrl::windowClass_ == 0) {
		DeviceCtrl::windowClass_ = Window::registerClass(DeviceCtrl::windowClassName_, NULL, 0);
	}
	return DeviceCtrl::windowClassName_;
}

void DeviceCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	Ctrl::create(parent, name, style, exStyle);
	var dev = deviceExt_->device();

	loadBackgroundImage();
	if (hBackgroundImage_) {
		background_.create(this, "bgbitmap", WS_CLIPSIBLINGS | SS_BITMAP);
		background_.set(rect_);
		SYSPR(SendMessage(background_.hWnd(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBackgroundImage_));
	}

	createControls();

	RECT rect = { borderWidth_, borderWidth_, 0, 0 };
	if (dev->presetBank() != NULL) {
		int presetCtrlWidth = 8 * CHARWIDTH;
		int presetButtonWidth = CHARWIDTH;
		toolbarCtrl_.create(this, "toolbar", SS_GRAYFRAME);
		toolbarCtrl_.set(0, 0, borderWidth_ + presetCtrlWidth + 2*(borderWidth_ + presetButtonWidth) + borderWidth_, borderWidth_ + CHARHEIGHT + borderWidth_);
		// add combobox for soundbank
		var cx = 0, cy = 0;
		presetCtrl_.create(&toolbarCtrl_, "preset", CBS_DROPDOWN | WS_VSCROLL);
		presetCtrl_.onAddItem([](ComboboxCtrl* cb, void* item) {});
		//presetCtrl_.onSelectItem([](ComboboxCtrl* cb, int ix, void* item) {});
		presetCtrl_.onSelectItem(DeviceCtrl::onSelectPreset);
		presetCtrl_.select(dev->preset());
		presetCtrl_.set(cx, cy, presetCtrlWidth, CHARHEIGHT);
		cx += presetCtrl_.rect().right, cy += presetCtrl_.rect().bottom;
		// add buttons add/del
		addPresetButton_.create(&toolbarCtrl_, "Add Pr", BS_CENTER | BS_TEXT);
		SYSPR(SetWindowText(addPresetButton_.hWnd(), "+"));
		addPresetButton_.onLeftUp(DeviceCtrl::onAddPreset);
		addPresetButton_.set(cx, cy, presetButtonWidth, CHARHEIGHT);
		cx += addPresetButton_.rect().right, cy += addPresetButton_.rect().bottom;

		removePresetButton_.create(&toolbarCtrl_, "Del Pr", BS_CENTER | BS_TEXT);
		SYSPR(SetWindowText(removePresetButton_.hWnd(), "-"));
		removePresetButton_.onLeftUp(DeviceCtrl::onRemovePreset);
		removePresetButton_.set(cx, cy, presetButtonWidth, CHARHEIGHT);
		//cx += removePresetButton_.rect().right, cy += removePresetButton_.rect().bottom;

		//updateSoundbank();
	}
}

void DeviceCtrl::createControls() {
	var dev = deviceExt_->device();
	var count = dev->inputCount();
	inputControls_ = NEWARR(InputCtrl, count);
	char lbl[16];
	int x = borderWidth_;
	int y = borderWidth_;
	if (toolbarCtrl_.hWnd() != NULL) y += toolbarCtrl_.rect().bottom;
	int width = borderWidth_, height = borderWidth_;
	for (var ii = 0; ii < count; ii++) {
		var ctrl = &inputControls_[ii];
		sprintf_s(lbl, 16, "input%x", ii);
		ctrl->create(this, lbl);
		ctrl->ctrlId(ii);
		ctrl->label(lbl);
		ctrl->input(dev->getInput(ii));
		ctrl->type(InputCtrlType::Knob);
		ctrl->set(x, y, 0, 0);
		x += borderWidth_ + ctrl->rect().right;
		if (height < ctrl->rect().bottom) {
			height = ctrl->rect().bottom;
		}
	}
	height += borderWidth_;
	setSize(x, height);
}

void DeviceCtrl::initFromStream(Stream* stream) {
	stream->rewind();
	int width = borderWidth_, height = borderWidth_;
	while (!stream->eos()) {
		var item = (LayoutItem*)stream->cursor();
		stream->seek(sizeof(LayoutItem), SEEK_CUR);
		var ctrl = &inputControls_[item->ctrlId];
		ctrl->create(this, item->label);
		ctrl->ctrlId(item->ctrlId);
		ctrl->input(deviceExt_->device()->getInput(item->ctrlId));
		ctrl->label(item->label);
		ctrl->type(item->type);
		ctrl->move(borderWidth_ + item->position.x, borderWidth_ + item->position.y);
		var right = ctrl->offset().x + ctrl->rect().right;
		if (width < right) {
			width = right;
		}
		var bottom = ctrl->offset().y + ctrl->rect().bottom;
		if (height < bottom) {
			height = bottom;
		}
	}
	width += borderWidth_;
	height += borderWidth_;
	setSize(width, height);
	SYSPR(InvalidateRect(hWnd_, NULL, true));

}

int DeviceCtrl::inputCount() {
	return deviceExt_->device()->inputCount();
}

InputCtrl* DeviceCtrl::getInput(int id) {
	return &inputControls_[id];
}

LRESULT DeviceCtrl::onSize(RECT& rect, WPARAM state) {
	LRESULT result = 0;

	return result;
}

void DeviceCtrl::onSelectPreset(ComboboxCtrl* cb, int ix, void* item) {
	var ctrl = (DeviceCtrl*)cb->parent();
	//TODO: DeviceExt::presetBankSetter(ctrl->deviceExt_, ix, NULL);
}

LRESULT DeviceCtrl::onAddPreset(Window* button, POINT& pos, WPARAM state) {
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

LRESULT DeviceCtrl::onRemovePreset(Window* button, POINT& pos, WPARAM state) {
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
	var dev = ctrl->deviceExt_->device();
	dev->setPresetBank(pb);
	//ctrl->updateSoundbank();
	return 1;
}

int DeviceCtrl::presetAdder(void* obj, Stream* preset, void* args) {
	var ctrl = (DeviceCtrl*)obj;
	var ext = ctrl->deviceExt_;
	// update preset combobox

	return 1;
}

int DeviceCtrl::presetRemover(void* obj, int ix, void* args) {
	var ctrl = (DeviceCtrl*)obj;
	var ext = ctrl->deviceExt_;
	// update preset combobox

	return 1;
}

int DeviceCtrl::presetSetter(void* obj, int ix, void* unused) {
	var ctrl = (DeviceCtrl*)obj;
	var ext = ctrl->deviceExt_;
	LRESULT res = 0;
	ext->device()->setPreset(ix);
	SYSPR2(SendMessage(ctrl->presetCtrl_.hWnd(), CB_GETLBTEXT, res, ix), LB_ERR);
	ctrl->presetCtrl_.select(ix);
	return 1;
}

