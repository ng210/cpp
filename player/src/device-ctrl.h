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

	class DeviceCtrl;
	typedef DeviceCtrl* (DEVICECONTROLCREATOR)(DeviceExt*);

	typedef struct LayoutItem {
		byte ctrlId;
		char label[16];
		word x, y;
		InputCtrlType type;
		byte scale;
	} LayoutItem;

	class DeviceCtrl : public Ctrl {
			static char* windowClassName_;
			static ATOM windowClass_;
	protected: PROP_R(DeviceExt*, deviceExt);
	protected: StaticCtrl toolbarCtrl_;
	protected: StaticCtrl background_;
	protected: RECT toolbarRect_;
	protected: RECT inputRect_;
	protected: PROP_R(LayoutItem*, layout);

	protected: PROP(int, borderWidth);
	protected: PROP(HANDLE, hBackgroundImage);
	protected: PROP_R(InputCtrl*, inputControls);
	protected: PROP_R(int, inputCount);
	protected: PROP_R(ComboboxCtrl, presetCtrl);
	protected: PROP_R(ButtonCtrl, addPresetButton);
	protected: PROP_R(ButtonCtrl, removePresetButton);
	protected: PROP_R(int, inputSpacing);

	public:
		DWORD colors[4];

		DeviceCtrl(DeviceExt* deviceExt);
		virtual ~DeviceCtrl();

		//char* const registerWindowClass();
		virtual void create(Window* parent, char* name);
		void create(Window* parent, char* name, Stream* stream);
		//LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		Stream* createDefaultLayout();

		virtual void initLayoutFromStream(Stream* stream);
		virtual void loadBackgroundImage() = 0;

		InputCtrl* getInput(int id);

		void setSize(int width, int height);
		void resize();

		void updatePresetBankCtrl();

		static CBSELECTITEMPROC onSelectPreset;
		static MOUSEEVENTPROC onAddPreset, onRemovePreset;

		static int presetBankSetter(void*, PresetBank*, void* = NULL);
		//static int presetAdder(void*, Stream*, void* = NULL);
		//static int presetRemover(void*, int, void* = NULL);
		static int presetSetter(void*, int, void* = NULL);

		private: static Map* deviceControls_;
		public:
			static void addDeviceControl(int type, DEVICECONTROLCREATOR* deviceCtrlCreator);
			static DeviceCtrl* getDeviceControl(DeviceExt* deviceExt);

			static void cleanUp();
	};

}

#endif