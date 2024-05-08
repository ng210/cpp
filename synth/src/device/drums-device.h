#ifndef __DRUMS_DEVICE_H
#define __DRUMS_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/module/drums.h"
#include "synth/src/device/generic-drum-device.h"

using namespace PLAYER;
namespace SYNTH {

	typedef struct DrumsStageInputs {
		Input bankId;
		Input prgId;
		InputF8 pan;
		InputF8 send[4];
	} DrumsStageInputs;
	#define DrumsStageInputSize (3+4)
	typedef struct DrumsInputs {
		DrumsStageInputs drums[DRUMS_COUNT];
	} DrumsInputs;
	#define DrumsInputSize (DRUMS_COUNT*DrumsStageInputSize)
	typedef enum DrumsInputIds {
		DrumsCh1bnk, DrumsCh1prg, DrumsCh1pan, DrumsCh1send1, DrumsCh1send2, DrumsCh1send3, DrumsCh1send4,
		DrumsCh2bnk, DrumsCh2prg, DrumsCh2pan, DrumsCh2send1, DrumsCh2send2, DrumsCh2send3, DrumsCh2send4,
		DrumsCh3bnk, DrumsCh3prg, DrumsCh3pan, DrumsCh3send1, DrumsCh3send2, DrumsCh3send3, DrumsCh3send4,
		DrumsCh4bnk, DrumsCh4prg, DrumsCh4pan, DrumsCh4send1, DrumsCh4send2, DrumsCh4send3, DrumsCh4send4,
		DrumsCh5bnk, DrumsCh5prg, DrumsCh5pan, DrumsCh5send1, DrumsCh5send2, DrumsCh5send3, DrumsCh5send4,
		DrumsCh6bnk, DrumsCh6prg, DrumsCh6pan, DrumsCh6send1, DrumsCh6send2, DrumsCh6send3, DrumsCh6send4,
		DrumsCh7bnk, DrumsCh7prg, DrumsCh7pan, DrumsCh7send1, DrumsCh7send2, DrumsCh7send3, DrumsCh7send4,
		DrumsCh8bnk, DrumsCh8prg, DrumsCh8pan, DrumsCh8send1, DrumsCh8send2, DrumsCh8send3, DrumsCh8send4
	} DrumsInputIds;

	class DrumsDevice : public ModuleDevice {
	protected: GenericDrumDevice* drumDevices_[DRUMS_COUNT];
	protected: PROP_R(PArray, drumPresetBanks);
	public:
		DrumsInputs drumsInputs;

		DrumsDevice(SynthAdapter* adapter, Player* player);
		virtual ~DrumsDevice();

		void processCommand(byte cmd, byte*& cursor);

		int getPresetBankSize();
		PresetBank* getDefaultPresetBank();

		Drums* drums() { return (Drums*)object_; }
		GenericDrumDevice** drum() { return drumDevices_; }

		static int presetSetter(void* obj, int presetId, void* args);
	};
}
#endif