#ifndef __PLAYER_DEVICE_CTRL_H
#define __PLAYER_DEVICE_CTRL_H

#include "win/buttonctrl.h"
#include "win/comboboxctrl.h"
#include "player/src/device-ext.h"
#include "player/src/input-ctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

namespace PLAYER {

	typedef struct LayoutItem {
		byte ctrlId;
		char label[16];
		InputCtrlType type;
	} LayoutItem;

	typedef enum LayoutControl {
		LayoutEnd = 255,
		LayoutHorizontalGap = 254,
		LayoutVerticalGap = 253,
		LayoutNextColumn = 252,
		LayoutBlank = 251,
		LayoutNewColumn = 250,
	} LayoutControl;

	class DeviceCtrl : public Ctrl {
	private: static WndClass WndClass_;
	protected: PROP_R(DeviceExt*, deviceExt);

	protected: PROP_R(ComboboxCtrl, presetCtrl);
	protected: PROP_R(ButtonCtrl, addPresetButton);
	protected: PROP_R(ButtonCtrl, removePresetButton);

	public:
		DeviceCtrl(DeviceExt* deviceExt);
		virtual ~DeviceCtrl();

		WndClass getWindowClass();
		void createWindow(Window* parent, char* name, LONG style = 0, DWORD exStyle = 0);

		virtual void initFromStream(Stream* data, int size = 100, unsigned long* colors = NULL);

		LRESULT onSize(RECT& rect, WPARAM state);

		static CBSELECTITEMPROC onSelectProgram;
		static MOUSEEVENTPROC onAddProgram, onRemoveProgram;
		static int presetBankSetter(void*, PresetBank*, void* = NULL);
		static int presetSetter(void*, int, void* = NULL);
	};

}

#endif