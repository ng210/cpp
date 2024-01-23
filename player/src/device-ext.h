#ifndef __PLAYER_DEVICE_EXT_H
#define __PLAYER_DEVICE_EXT_H

#include "player/src/input.h"
#include "player/src/device.h"
#include "player/src/sequence.h"
#include "player/src/preset-bank.h"

NS_FW_BASE_USE

namespace PLAYER {
	class DeviceExt {
	protected: PROP_R(Device*, device);
		Input* inputs_;
	public:
		DeviceExt(Device* device);
		virtual ~DeviceExt();
		void initialize(Stream* stream);

		virtual void createInputs();
		virtual Input* getInput(int id);

		virtual void writeToStream(Stream* stream);
		virtual void writePresetToStream(Stream* stream);

		virtual Stream* makeCommand(byte command, ...);
		virtual void makeCommandImpl(int command, Stream* stream, va_list args) = 0;
		virtual int getCommandSize(byte* cmd) = 0;

		virtual Sequence* createDefaultSequence();

		static PresetBank* loadPresetBank(const char* presetBankPath);
	};
}

#endif