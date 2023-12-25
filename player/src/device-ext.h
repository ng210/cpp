#ifndef __PLAYER_DEVICE_EXT_H
#define __PLAYER_DEVICE_EXT_H

#include "base/handler.h"
#include "player/src/device.h"
#include "player/src/preset-bank.h"

NS_FW_BASE_USE

namespace PLAYER {
	class DeviceExt : public Device {
	protected: PROP_R(InputBase*, inputs);
	protected: PROP_R(int, inputCount);
	protected: PROP_R(PresetBank*, presetBank);
	protected: PROP_R(int, preset);
	
	public:
		DeviceExt(void* object, Adapter* adapter);
		~DeviceExt();

		Handler<PresetBank*> setPresetBank;
		Handler<int> setPreset;

		virtual void initializeFromStream(byte** pData);
		virtual int writeToStream(Stream* stream);
		virtual int writePreset(byte* pData);

		virtual Stream* makeCommand(byte command, ...);
		virtual void makeCommandImpl(int command, Stream* stream, va_list args) = 0;
		virtual int getCommandSize(byte* cmd) = 0;

		virtual Sequence* createDefaultSequence();

		static int presetBankSetter(void*, PresetBank*, void* = NULL);
		static int presetSetter(void*, int, void* = NULL);
		static byte* loadPresetBanks(const char* presetBankPath);
	};
}

#endif