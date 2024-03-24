//#include "player/src/device-ctrl.h"
//
//using namespace PLAYER;
//
//#define CHARWIDTH 10
//#define CHARHEIGHT 24
//
//char* DeviceCtrl::windowClassName_ = "DeviceCtrl";
//ATOM DeviceCtrl::windowClass_;
//
//DeviceCtrl::DeviceCtrl(DeviceExt* deviceExt) {
//	deviceExt_ = deviceExt;
//	inputControls_ = NULL;
//	borderWidth_ = 2;
//	hBackgroundImage_ = NULL;
//}
//
//DeviceCtrl::~DeviceCtrl() {
//	DELARR(inputControls_);
//}
//
////char* const DeviceCtrl::registerWindowClass() {
////	if (DeviceCtrl::windowClass_ == 0) {
////		WNDCLASSEX wndClassEx = {
////			sizeof(WNDCLASSEX),				// cbSize;
////			0,					// style;
////			&Window::wndProcWrapper,		// lpfnWndProc;
////			0,                              // cbClsExtra;
////			0,                              // cbWndExtra;
////			NULL,							// hInstance;
////			NULL,                           // hIcon;
////			LoadCursor(NULL, IDC_HAND),		// hCursor;
////			(HBRUSH)COLOR_WINDOW,		// hbrBackground;
////			NULL,                           // lpszMenuName;
////			DeviceCtrl::windowClassName_,	// lpszClassName;
////			NULL                            // hIconSm;
////		};
////		DeviceCtrl::windowClass_ = Window::registerClass(wndClassEx);
////		//DeviceCtrl::windowClass_ = Window::registerClass(DeviceCtrl::windowClassName_, NULL, 0);
////	}
////	return DeviceCtrl::windowClassName_;
////}
//
//void DeviceCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
//	Ctrl::create(parent, name, style, exStyle);
//	var dev = deviceExt_->device();
//
//	setSize(200, 100);
//
//	loadBackgroundImage();
//	if (hBackgroundImage_) {
//		background_.create(this, "bgbitmap", SS_BITMAP);
//		background_.set(rect_);
//		SYSPR(SendMessage(background_.hWnd(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBackgroundImage_));
//	}
//
//	createControls();
//
//	RECT rect = { borderWidth_, borderWidth_, 0, 0 };
//	if (dev->presetBank() != NULL) {
//		int presetCtrlWidth = 8 * CHARWIDTH;
//		int presetButtonWidth = CHARWIDTH;
//		toolbarCtrl_.create(this, "toolbar", SS_GRAYFRAME);
//		toolbarCtrl_.set(0, 0, borderWidth_ + presetCtrlWidth + 2 * (borderWidth_ + presetButtonWidth) + borderWidth_, borderWidth_ + CHARHEIGHT + borderWidth_);
//		// add combobox for soundbank
//		var cx = 0, cy = 0;
//		presetCtrl_.create(&toolbarCtrl_, "preset", CBS_DROPDOWN | WS_VSCROLL);
//		presetCtrl_.onAddItem([](ComboboxCtrl* cb, void* item) {});
//		//presetCtrl_.onSelectItem([](ComboboxCtrl* cb, int ix, void* item) {});
//		presetCtrl_.onSelectItem(DeviceCtrl::onSelectPreset);
//		presetCtrl_.select(dev->preset());
//		presetCtrl_.set(cx, cy, presetCtrlWidth, CHARHEIGHT);
//		cx += presetCtrl_.rect().right, cy += presetCtrl_.rect().bottom;
//		// add buttons add/del
//		addPresetButton_.create(&toolbarCtrl_, "Add Pr", BS_CENTER | BS_TEXT);
//		SYSPR(SetWindowText(addPresetButton_.hWnd(), "+"));
//		addPresetButton_.onLeftUp(DeviceCtrl::onAddPreset);
//		addPresetButton_.set(cx, cy, presetButtonWidth, CHARHEIGHT);
//		cx += addPresetButton_.rect().right, cy += addPresetButton_.rect().bottom;
//
//		removePresetButton_.create(&toolbarCtrl_, "Del Pr", BS_CENTER | BS_TEXT);
//		SYSPR(SetWindowText(removePresetButton_.hWnd(), "-"));
//		removePresetButton_.onLeftUp(DeviceCtrl::onRemovePreset);
//		removePresetButton_.set(cx, cy, presetButtonWidth, CHARHEIGHT);
//		//cx += removePresetButton_.rect().right, cy += removePresetButton_.rect().bottom;
//
//		//updateSoundbank();
//	}
//}
//
//void DeviceCtrl::createControls() {
//	var dev = deviceExt_->device();
//	var count = dev->inputCount();
//	inputControls_ = NEWARR(InputCtrl, count);
//	char lbl[16];
//	int x = borderWidth_;
//	int y = borderWidth_;
//	if (toolbarCtrl_.hWnd() != NULL) y += toolbarCtrl_.rect().bottom;
//	int width = borderWidth_, height = borderWidth_;
//	for (var ii = 0; ii < count; ii++) {
//		var ctrl = &inputControls_[ii];
//		sprintf_s(lbl, 16, "input%x", ii);
//		ctrl->create(this, lbl);
//		ctrl->ctrlId(ii);
//		ctrl->label(lbl);
//		ctrl->input(dev->getInput(ii));
//		ctrl->type(InputCtrlType::Knob);
//		ctrl->set(x, y, 0, 0);
//		x += borderWidth_ + ctrl->rect().right;
//		if (height < ctrl->rect().bottom) {
//			height = ctrl->rect().bottom;
//		}
//	}
//	height += borderWidth_;
//	//setSize(x, height);
//}
//
//void DeviceCtrl::initFromStream(Stream* stream) {
//	stream->rewind();
//	int width = borderWidth_, height = borderWidth_;
//	while (!stream->eos()) {
//		var item = (LayoutItem*)stream->cursor();
//		stream->seek(sizeof(LayoutItem), SEEK_CUR);
//		var ctrl = &inputControls_[item->ctrlId];
//		ctrl->create(this, item->label);
//		ctrl->ctrlId(item->ctrlId);
//		ctrl->input(deviceExt_->device()->getInput(item->ctrlId));
//		ctrl->label(item->label);
//		ctrl->type(item->type);
//		ctrl->move(borderWidth_ + item->position.x, borderWidth_ + item->position.y);
//		var right = ctrl->offset().x + ctrl->rect().right;
//		if (width < right) {
//			width = right;
//		}
//		var bottom = ctrl->offset().y + ctrl->rect().bottom;
//		if (height < bottom) {
//			height = bottom;
//		}
//	}
//	width += borderWidth_;
//	height += borderWidth_;
//	setSize(width, height);
//	SYSPR(InvalidateRect(hWnd_, NULL, true));
//
//}
//
//int DeviceCtrl::inputCount() {
//	return deviceExt_->device()->inputCount();
//}
//
//InputCtrl* DeviceCtrl::getInput(int id) {
//	return &inputControls_[id];
//}
//
//LRESULT DeviceCtrl::onSize(RECT& rect, WPARAM state) {
//	LRESULT result = 0;
//
//	return result;
//}
//
//void DeviceCtrl::onSelectPreset(ComboboxCtrl* cb, int ix, void* item) {
//	var ctrl = (DeviceCtrl*)cb->parent();
//	//TODO: DeviceExt::presetBankSetter(ctrl->deviceExt_, ix, NULL);
//}
//
//LRESULT DeviceCtrl::onAddPreset(Window* button, POINT& pos, WPARAM state) {
//	var devCtrl = (DeviceCtrl*)button->parent();
//	var device = (Device*)devCtrl->deviceExt_;
//	char name[16];
//	devCtrl->presetCtrl_.getText(name, 16);
//	var pb = device->presetBank();
//	var ix = pb->indexOf(name);
//	if (ix == -1) {
//		// add new preset
//		var prg = NEW_(Stream, pb->bankSize());
//		devCtrl->deviceExt_->writePresetToStream(prg);
//		pb->add(name, prg->data());
//		DEL_(prg);
//		//devCtrl->updateSoundbank();
//	}
//	else {
//		var prg = NEW_(Stream, (byte*)pb->get(&name[0]), pb->bankSize());
//		devCtrl->deviceExt_->writePresetToStream(prg);
//	}
//	SYSFN2(ix, (int)SendMessage(devCtrl->presetCtrl_.hWnd(), CB_FINDSTRINGEXACT, 0, (LPARAM)name), CB_ERR);
//	device->setPreset(ix);
//	return 1;
//}
//
//LRESULT DeviceCtrl::onRemovePreset(Window* button, POINT& pos, WPARAM state) {
//	var devCtrl = (DeviceCtrl*)button->parent();
//	var dev = (Device*)devCtrl->deviceExt_;
//	char name[16];
//	devCtrl->presetCtrl_.getText(name, 16);
//	var sb = dev->presetBank();
//	if (sb->size() > 1) {
//		var ix = sb->indexOf(name);
//		if (ix != -1) {
//			if (dev->preset() == ix) {
//				//TODO: DeviceExt::presetSetter(dev, ix > 0 ? 0 : 1, NULL);
//			}
//			// remove preset
//			Key key(&name);
//			sb->remove(key);
//			//devCtrl->updateSoundbank();
//		}
//	}
//	return 1;
//}
//
//int DeviceCtrl::presetBankSetter(void* obj, PresetBank* pb, void* unused) {
//	var ctrl = (DeviceCtrl*)obj;
//	var dev = ctrl->deviceExt_->device();
//	dev->setPresetBank(pb);
//	//ctrl->updateSoundbank();
//	return 1;
//}
//
//int DeviceCtrl::presetAdder(void* obj, Stream* preset, void* args) {
//	var ctrl = (DeviceCtrl*)obj;
//	var ext = ctrl->deviceExt_;
//	// update preset combobox
//
//	return 1;
//}
//
//int DeviceCtrl::presetRemover(void* obj, int ix, void* args) {
//	var ctrl = (DeviceCtrl*)obj;
//	var ext = ctrl->deviceExt_;
//	// update preset combobox
//
//	return 1;
//}
//
//int DeviceCtrl::presetSetter(void* obj, int ix, void* unused) {
//	var ctrl = (DeviceCtrl*)obj;
//	var ext = ctrl->deviceExt_;
//	LRESULT res = 0;
//	ext->device()->setPreset(ix);
//	SYSPR2(SendMessage(ctrl->presetCtrl_.hWnd(), CB_GETLBTEXT, res, ix), LB_ERR);
//	ctrl->presetCtrl_.select(ix);
//	return 1;
//}


#include "player/src/device-ctrl.h"

using namespace PLAYER;

#define CHARWIDTH 8
#define CHARHEIGHT 24

char* DeviceCtrl::windowClassName_ = "DeviceCtrl";
ATOM DeviceCtrl::windowClass_;

Map* DeviceCtrl::deviceControls_ = NULL;

DeviceCtrl::DeviceCtrl(DeviceExt* deviceExt) {
	deviceExt_ = deviceExt;
	inputControls_ = NULL;
	inputCount_ = 0;
	borderWidth_ = 2;
	hBackgroundImage_ = NULL;
	// background, foreground, frame, text
	colors[0] = 0x00404040; colors[1] = 0x00c0c0c0;
	colors[2] = 0x00808080; colors[3] = 0x00000000;
	toolbarRect_ = { 0, 0, 0, 0 };
	inputRect_ = { 0, 0, 0, 0 };
	inputSpacing_ = 1;
}

DeviceCtrl::~DeviceCtrl() {
	DELARR(inputControls_);
	FREE(layout_);
}

//char* const DeviceCtrl::registerWindowClass() {
//	if (DeviceCtrl::windowClass_ == 0) {
//		DeviceCtrl::windowClass_ = Window::registerClass(DeviceCtrl::windowClassName_, NULL, 0);
//	}
//	return DeviceCtrl::windowClassName_;
//}

//void DeviceCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
//	Ctrl::create(parent, name, style, exStyle);
//	var dev = deviceExt_->device();
//	int wi = 0, he = 0;
//
//	loadBackgroundImage();
//	if (hBackgroundImage_) {
//		background_.create(this, "bgbitmap", WS_CLIPSIBLINGS | SS_BITMAP);
//		background_.set(rect_);
//		SYSPR(SendMessage(background_.hWnd(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBackgroundImage_));
//	}
//
//
//	RECT rect = { borderWidth_, borderWidth_, 0, 0 };
//	if (dev->presetBank() != NULL) {
//		int x = borderWidth_, y = borderWidth_;
//		int presetCtrlWidth = 12 * CHARWIDTH;
//		int presetButtonWidth = 2 * CHARWIDTH;
//
//		toolbarCtrl_.create(this, "toolbar", SS_GRAYFRAME);
//		toolbarCtrl_.set(x, y, borderWidth_ + presetCtrlWidth + 2 * (borderWidth_ + presetButtonWidth) + borderWidth_, borderWidth_ + CHARHEIGHT + borderWidth_);
//
//#pragma region Add combobox for soundbank
//		x = 0;
//		presetCtrl_.create(&toolbarCtrl_, "preset", CBS_DROPDOWN | WS_VSCROLL);
//		presetCtrl_.set(x, 0, presetCtrlWidth, 4 * CHARHEIGHT);
//		x += presetCtrl_.rect().right;
//		// set font
//		//var hFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
//		//SYSPR(SendMessage(presetCtrl_.edit(), WM_SETFONT, (WPARAM)hFont, true));
//		//SYSPR(SendMessage(presetCtrl_.list(), WM_SETFONT, (WPARAM)hFont, true));
//		var keys = dev->presetBank()->keys();
//		for (var ki = 0; ki < keys->length(); ki++) {
//			presetCtrl_.addItem((char*)keys->get(ki));
//		}
//		presetCtrl_.onSelectItem(DeviceCtrl::onSelectPreset);
//		//presetCtrl_.onAddItem([](ComboboxCtrl* cb, void* item) {});
//		presetCtrl_.select(dev->preset());
//#pragma	endregion
//
//#pragma region Add buttons
//		addPresetButton_.create(&toolbarCtrl_, "Add Pr", BS_CENTER | BS_TEXT);
//		addPresetButton_.set(x, 0, presetButtonWidth, CHARHEIGHT);
//		x += addPresetButton_.rect().right + borderWidth_;
//		SYSPR(SetWindowText(addPresetButton_.hWnd(), "+"));
//		addPresetButton_.onLeftUp(DeviceCtrl::onAddPreset);
//
//		removePresetButton_.create(&toolbarCtrl_, "Del Pr", BS_CENTER | BS_TEXT);
//		removePresetButton_.set(x, 0, presetButtonWidth, CHARHEIGHT);
//		x += removePresetButton_.rect().right + borderWidth_;
//		SYSPR(SetWindowText(removePresetButton_.hWnd(), "-"));
//		removePresetButton_.onLeftUp(DeviceCtrl::onRemovePreset);
//		//updateSoundbank();
//
//		wi = toolbarCtrl_.rect().right;
//		he = toolbarCtrl_.rect().bottom + borderWidth_;
//#pragma endregion
//	}
//
//	inputCount_ = dev->inputCount();
//	inputControls_ = NEWARR(InputCtrl, inputCount_);
//	char lbl[16];
//	for (var ii = 0; ii < inputCount_; ii++) {
//		var ctrl = &inputControls_[ii];
//		sprintf_s(lbl, 16, "input%x", ii);
//		ctrl->type(InputCtrlType::Knob);
//		ctrl->create(this, lbl);
//		ctrl->input(deviceExt_->device()->getInput(ii));
//		//ctrl->show(SW_HIDE);
//		ctrl->set(0, 0, 0, 0);
//	}
//
//	setSize(wi, he + 100);
//
//	//onSize(rect_, 0);
//}

void DeviceCtrl::create(Window* parent, char* name) {
	create(parent, name, NULL);
}

void DeviceCtrl::create(Window* parent, char* name, Stream* stream) {
	Ctrl::create(parent, name);
	var dev = deviceExt_->device();

	loadBackgroundImage();
	if (hBackgroundImage_) {
		background_.create(this, "bgbitmap", SS_BITMAP/* | SS_REALSIZECONTROL*/);
		//SYSPR(SendMessage(background_.hWnd(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBackgroundImage_));
	}

	RECT rect = { borderWidth_, borderWidth_, 0, 0 };
	if (dev->presetBank() != NULL) {
		toolbarCtrl_.create(this, "toolbar", SS_GRAYFRAME);

		#pragma region Add combobox for soundbank
		presetCtrl_.create(&toolbarCtrl_, "preset", CBS_DROPDOWN | WS_VSCROLL);
		// set font
		//var hFont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
		//SYSPR(SendMessage(presetCtrl_.edit(), WM_SETFONT, (WPARAM)hFont, true));
		//SYSPR(SendMessage(presetCtrl_.list(), WM_SETFONT, (WPARAM)hFont, true));
		updatePresetBankCtrl();
		presetCtrl_.onSelectItem(DeviceCtrl::onSelectPreset);
		dev->setPreset.add(this, presetSetter);
		//presetCtrl_.onAddItem([](ComboboxCtrl* cb, void* item) {});
		presetCtrl_.select(dev->preset());
		#pragma	endregion

		#pragma region Add buttons
		addPresetButton_.create(&toolbarCtrl_, "Add Pr", BS_CENTER | BS_TEXT);
		SYSPR(SetWindowText(addPresetButton_.hWnd(), "+"));
		addPresetButton_.onLeftUp(DeviceCtrl::onAddPreset);

		removePresetButton_.create(&toolbarCtrl_, "Del Pr", BS_CENTER | BS_TEXT);
		SYSPR(SetWindowText(removePresetButton_.hWnd(), "-"));
		removePresetButton_.onLeftUp(DeviceCtrl::onRemovePreset);
		//updateSoundbank();
		#pragma endregion
	}

	inputCount_ = dev->inputCount();
	inputControls_ = NEWARR(InputCtrl, inputCount_);
	char lbl[16];
	for (var ii = 0; ii < inputCount_; ii++) {
		var ctrl = &inputControls_[ii];
		sprintf_s(lbl, 16, "input%x", ii);
		ctrl->create(this, lbl);
		ctrl->show(SW_HIDE);
	}

	if (stream == NULL) {
		stream = createDefaultLayout();
		toolbarRect_.left = borderWidth_;
		toolbarRect_.top = borderWidth_;
		inputRect_.left = borderWidth_;
		inputRect_.top = toolbarRect_.top + borderWidth_ + CHARHEIGHT + borderWidth_;
		initLayoutFromStream(stream);
		DEL_(stream);
	}
	else {
		initLayoutFromStream(stream);
	}

	//onSize(rect_, NULL);
	//SYSPR(InvalidateRect(hWnd_, NULL, true));
}

//LRESULT CALLBACK DeviceCtrl::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//	LRESULT ret = 0;
//	switch (uMsg) {
//		case WM_CTLCOLORLISTBOX:
//			HWND ctrl; ctrl = (HWND)lParam;
//			HDC hDC; hDC = (HDC)wParam;
//			//if (ctrl == logCtrl_.hWnd()) {
//			//	// set color for log
//			//	SetTextColor(hDC, LOGTEXTCOLOR);
//			//	SetBkColor(hDC, LOGBGCOLOR);
//			//	return (INT_PTR)logBgBrush_;
//			//}
//			//else if (ctrl == testArea_.hWnd()) {
//			//	SetTextColor(hDC, TESTAREABGCOLOR);
//			//	return (INT_PTR)testAreaBgBrush_;
//			//}
//		default:
//			ret = Window::wndProc(hWnd, uMsg, wParam, lParam);
//	}
//	return ret;
//}

Stream* DeviceCtrl::createDefaultLayout() {
	// create default layout
	int x = 0;
	int y = 0;
	char lbl[16];
	var stream = NEW_(Stream, inputCount_ * sizeof(LayoutItem));
	for (var ii = 0; ii < inputCount_; ii++) {
		var ctrl = &inputControls_[ii];
		sprintf_s(lbl, 16, "input%x", ii);
		stream->writeByte(ii);
		stream->writeString(lbl);
		stream->writeWord(x);
		stream->writeWord(y);
		stream->writeByte(InputCtrlType::Knob);
		stream->writeByte(100);
		ctrl->type(InputCtrlType::Knob);
		ctrl->setScale(100);
		x += borderWidth_ + ctrl->rect().right;
	}
	stream->rewind();
	return stream;
}

void DeviceCtrl::initLayoutFromStream(Stream* stream) {
	if (layout_) FREE(layout_);
	layout_ = MALLOC(LayoutItem, inputCount_);
	memset(layout_, 0, sizeof(LayoutItem) * inputCount_);
	while (!stream->eos()) {
		var id = stream->readByte();
		var lbl = stream->readString();
		var px = stream->readWord();
		var py = stream->readWord();
		var type = stream->readByte();
		var scale = stream->readByte();

		var layoutItem = &layout_[id];
		layoutItem->ctrlId = id;
		strncpy(layoutItem->label, 16, lbl);
		layoutItem->x = px;
		layoutItem->y = py;
		layoutItem->type = (InputCtrlType)type;

		var ctrl = &inputControls_[id];
		ctrl->ctrlId(id);
		ctrl->label(lbl);
		ctrl->move(px, py);
		ctrl->type((InputCtrlType)(type & 0x0f));
		ctrl->input(deviceExt_->device()->getInput(id));
		ctrl->setColors(colors);
		ctrl->show(SW_SHOW);
		ctrl->showLabel(InputCtrlNoLabel - (type & InputCtrlNoLabel));
		ctrl->showValue(InputCtrlNoValue - (type & InputCtrlNoValue));
		ctrl->gap = inputSpacing_;
		ctrl->setScale(scale);
	}
	resize();
}

InputCtrl* DeviceCtrl::getInput(int id) {
	return &inputControls_[id];
}

void DeviceCtrl::setSize(int width, int height) {
	Window::setSize(width, height);
	//background_.setSize(rect_.right, rect_.bottom);
}

void DeviceCtrl::resize() {
	int presetCtrlWidth = 12 * CHARWIDTH;
	int presetButtonWidth = 3 * CHARWIDTH;

	if (toolbarCtrl_.hWnd()) {
		toolbarCtrl_.set(toolbarRect_.left, toolbarRect_.top, borderWidth_ + presetCtrlWidth + 2 * (borderWidth_ + presetButtonWidth) + borderWidth_, CHARHEIGHT);
		presetCtrl_.set(0, 0, presetCtrlWidth, 4 * CHARHEIGHT);
		var tx = presetCtrl_.rect().right + 2*borderWidth_;
		addPresetButton_.set(tx, borderWidth_, presetButtonWidth - 2 * borderWidth_, CHARHEIGHT - 2 * borderWidth_);
		tx += addPresetButton_.rect().right + borderWidth_;
		removePresetButton_.set(tx, borderWidth_, presetButtonWidth - 2 * borderWidth_, CHARHEIGHT - 2 * borderWidth_);
		//tx += removePresetButton_.rect().right + borderWidth_;
	}

	// move inputs
	int width = borderWidth_, height = borderWidth_;
	var ix = inputRect_.left;
	var iy = inputRect_.top;
	for (var ii = 0; ii < inputCount_; ii++) {
		InputCtrl& inputCtrl = inputControls_[ii];
		var cx = layout_[ii].x + ix;
		var cy = layout_[ii].y + iy;
		inputCtrl.move(cx, cy);
		var right = cx + inputCtrl.rect().right + borderWidth_;
		if (right > width) width = right;
		var bottom = cy + inputCtrl.rect().bottom + borderWidth_;
		if (height < bottom) {
			height = bottom;
		}
	}
	inputRect_.right = width;
	inputRect_.bottom = height;

	if (width < toolbarCtrl_.rect().right) width = toolbarCtrl_.rect().right;

	if (hBackgroundImage_) {
		background_.setSize(width, height);
		SYSPR(SendMessage(background_.hWnd(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBackgroundImage_));
	}

	if (rect_.right != width || rect_.bottom != height) {
		setSize(width, height);
	}	
}

void DeviceCtrl::updatePresetBankCtrl() {
	var keys = deviceExt_->device()->presetBank()->keys();
	presetCtrl_.reset();
	for (var ki = 0; ki < keys->length(); ki++) {
		presetCtrl_.addItem((char*)keys->get(ki));
	}

}

void DeviceCtrl::onSelectPreset(ComboboxCtrl* cb, int ix, void* item) {
	var devCtrl = (DeviceCtrl*)cb->parent()->parent();
	devCtrl->deviceExt_->device()->setPreset(ix);
	//TODO: DeviceExt::presetBankSetter(ctrl->deviceExt_, ix, NULL);
}

LRESULT DeviceCtrl::onAddPreset(Window* button, POINT& pos, WPARAM state) {
	var devCtrl = (DeviceCtrl*)button->parent()->parent();
	var device = devCtrl->deviceExt_->device();
	char name[16];
	devCtrl->presetCtrl_.getText(name, 16);
	if (fmw::strlen(name) > 0) {
		var pb = device->presetBank();
		var ix = pb->indexOf(name);
		if (ix == -1) {
			// add new preset
			var prg = NEW_(Stream, pb->bankSize());
			devCtrl->deviceExt_->writePresetToStream(prg);
			pb->add(name, prg->data());
			DEL_(prg);
			devCtrl->updatePresetBankCtrl();
			ix = pb->indexOf(name);
			device->setPreset(ix);
		}
		else {
			var prg = NEW_(Stream, pb->bankSize());
			devCtrl->deviceExt_->writePresetToStream(prg);
			fmw::memcpy(pb->get(&name[0]), prg->data(), pb->bankSize());
			DEL_(prg);
		}
		//SYSFN2(ix, (int)SendMessage(devCtrl->presetCtrl_.hWnd(), CB_FINDSTRINGEXACT, 0, (LPARAM)name), CB_ERR);
		device->setPreset(ix);
	}
	return 1;
}

LRESULT DeviceCtrl::onRemovePreset(Window* button, POINT& pos, WPARAM state) {
	var devCtrl = (DeviceCtrl*)button->parent()->parent();
	var device = devCtrl->deviceExt_->device();
	char name[16];
	devCtrl->presetCtrl_.getText(name, 16);
	var pb = device->presetBank();
	if (pb->size() > 0) {
		var ix = pb->indexOf(name);
		if (ix != -1) {
			// remove preset
			Key key(&name);
			pb->remove(key);
			devCtrl->updatePresetBankCtrl();
		}
		if (ix >= pb->size()) ix = (int)pb->size() - 1;
		//SYSFN2(ix, (int)SendMessage(devCtrl->presetCtrl_.hWnd(), CB_FINDSTRINGEXACT, 0, (LPARAM)name), CB_ERR);
		device->setPreset(ix);
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

//int DeviceCtrl::presetAdder(void* obj, Stream* preset, void* args) {
//	var ctrl = (DeviceCtrl*)obj;
//	var ext = ctrl->deviceExt_;
//	// update preset combobox
//
//	return 1;
//}
//
//int DeviceCtrl::presetRemover(void* obj, int ix, void* args) {
//	var ctrl = (DeviceCtrl*)obj;
//	var ext = ctrl->deviceExt_;
//	// update preset combobox
//
//	return 1;
//}

int DeviceCtrl::presetSetter(void* obj, int ix, void* unused) {
	var ctrl = (DeviceCtrl*)obj;
	var ext = ctrl->deviceExt_;
	LRESULT res = 0;
	//ext->device()->setPreset(ix);
	//SYSPR2(SendMessage(ctrl->presetCtrl_.hWnd(), CB_GETLBTEXT, res, ix), LB_ERR);
	ctrl->presetCtrl_.select(ix);
	return 1;
}

void DeviceCtrl::addDeviceControl(int key, DEVICECONTROLCREATOR* deviceExtCreator) {
	if (DeviceCtrl::deviceControls_ == NULL) {
		DeviceCtrl::deviceControls_ = NEW_(Map, sizeof(int), MAP_USE_REF, Map::hashingInt, Collection::compareInt);
	}
	DeviceCtrl::deviceControls_->add(key, deviceExtCreator);
}

DeviceCtrl* DeviceCtrl::getDeviceControl(DeviceExt* deviceExt) {
	DeviceCtrl* deviceCtrl = NULL;
	var device = deviceExt->device();
	if (DeviceCtrl::deviceControls_ != NULL) {
		var key = (device->adapter()->getInfo()->id << 8) + device->type();
		var creator = (DEVICECONTROLCREATOR*)DeviceCtrl::deviceControls_->get(key);
		if (creator != NULL) deviceCtrl = (*creator)(deviceExt);
	}
	return deviceCtrl;
}

void DeviceCtrl::cleanUp() {
	DEL_(DeviceCtrl::deviceControls_);
}
