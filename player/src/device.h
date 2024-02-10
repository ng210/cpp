#ifndef __PLAYER_DEVICE_H
#define __PLAYER_DEVICE_H

#include <stdarg.h>
#include "base/stream.h"
#include "player/src/adapter.h"
#include "player/src/input.h"
#include "player/src/preset-bank.h"

NS_FW_BASE_USE

namespace PLAYER {

	class Adapter;
	class Player;

	class Device {
	protected: PROP(void*, object);
	protected: PROP(int, type);
	protected: bool isActive_;
	protected: PROP_R(InputBase*, inputs);
	protected: PROP(int, inputCount);
	protected: PROP_R(PresetBank*, presetBank);
	protected: PROP_R(int, preset);

	//protected: PROP_R(float, refreshRate);

	protected: PROP(Adapter*, adapter);
	protected: PROP(Player*, player);
	//protected: PROP(byte*, initData);
	
	protected:
		Device();
		Device(Adapter* adapter, void* object = NULL);
	public:
		Handler<int> setPreset;
		Handler<PresetBank*> setPresetBank;

		virtual ~Device() {}

		virtual void initialize(byte** stream = NULL) = 0;

		virtual bool isActive();
		virtual void isActive(bool b);

		virtual InputBase* getInput(int id);
		virtual void setInput(int id, Value v);

		virtual int run(int ticks) = 0;
		virtual void processCommand(byte cmd, byte*& cursor) = 0;

		//virtual void setRefreshRate(float fps);

		static int presetBankSetter(void*, PresetBank*, void*);
		static int presetSetter(void*, int, void*);

	};
}
#endif