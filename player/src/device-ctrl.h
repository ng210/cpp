#ifndef __PLAYER_DEVICE_CTRL_H
#define __PLAYER_DEVICE_CTRL_H

#include "win/staticctrl.h"
#include "win/buttonctrl.h"
#include "win/comboboxctrl.h"
#include "player/src/device-ext.h"
#include "player/src/input-ctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

namespace PLAYER {

	typedef struct LayoutItem {
		byte ctrlId;
		POINT position;
		char label[16];
		InputCtrlType type;
	} LayoutItem;

	class DeviceCtrl : public Ctrl {
	private:
			static char* windowClassName_;
			static ATOM windowClass_;
			StaticCtrl toolbarCtrl_;
			StaticCtrl background_;

	protected: PROP_R(DeviceExt*, deviceExt);

	protected: PROP_R(int, borderWidth);
	protected: PROP(HANDLE, hBackgroundImage);
	protected: PROP_R(InputCtrl*, inputControls);
	protected: PROP_R(ComboboxCtrl, presetCtrl);
	protected: PROP_R(ButtonCtrl, addPresetButton);
	protected: PROP_R(ButtonCtrl, removePresetButton);

	public:
		DeviceCtrl(DeviceExt* deviceExt);
		virtual ~DeviceCtrl();

		char* const registerWindowClass();
		void create(Window* parent, char* name, LONG style = 0, DWORD exStyle = 0);

		virtual void createControls();

		virtual void initFromStream(Stream* data);
		virtual void loadBackgroundImage() = 0;

		int inputCount();
		InputCtrl* getInput(int id);

		LRESULT onSize(RECT& rect, WPARAM state);

		static CBSELECTITEMPROC onSelectPreset;
		static MOUSEEVENTPROC onAddPreset, onRemovePreset;

		static int presetBankSetter(void*, PresetBank*, void* = NULL);
		static int presetAdder(void*, Stream*, void* = NULL);
		static int presetRemover(void*, int, void* = NULL);
		static int presetSetter(void*, int, void* = NULL);
	};

}

#endif