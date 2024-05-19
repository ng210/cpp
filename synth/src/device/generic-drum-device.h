#ifndef __GENERIC_DRUMS_DEVICE_H
#define __GENERIC_DRUMS_DEVICE_H

#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"
#include "synth/src/module/generic-drum.h"


using namespace PLAYER;
namespace SYNTH {

	typedef struct GenericDrumInputs {
		Input type;
		DahrInputs dahr[4];
		FltInputs flt[2];
		InputF8 amp[6];
		InputF fre[6];

		// indirect inputs
		InputF8 length, tune, tone, click, snap;
	} GenericDrumInputs;

	#define GenericDrumInputSize (sizeof(byte) + 4*DahrInputSize + 2*FltInputSize + 6*(sizeof(byte) + sizeof(float)) + 5*sizeof(byte))

	typedef enum GenericDrumInputIds {
		GenDrumType,
		GenDrumDahrAmp1, GenDrumDahrDelay1, GenDrumDahrAtk1, GenDrumDahrHold1, GenDrumDahrRel1,
		GenDrumDahrAmp2, GenDrumDahrDelay2, GenDrumDahrAtk2, GenDrumDahrHold2, GenDrumDahrRel2,
		GenDrumDahrAmp3, GenDrumDahrDelay3, GenDrumDahrAtk3, GenDrumDahrHold3, GenDrumDahrRel3,
		GenDrumDahrAmp4, GenDrumDahrDelay4, GenDrumDahrAtk4, GenDrumDahrHold4, GenDrumDahrRel4,
		GenDrumFlt1Cut, GenDrumFlt1Res, GenDrumFlt1Mode,
		GenDrumFlt2Cut, GenDrumFlt2Res, GenDrumFlt2Mode,
		GenDrumAmp1, GenDrumAmp2, GenDrumAmp3, GenDrumAmp4, GenDrumAmp5, GenDrumAmp6,
		GenDrumFre1, GenDrumFre2, GenDrumFre3, GenDrumFre4, GenDrumFre5, GenDrumFre6,
		// indirect inputs
		GenDrumLength, GenDrumTune, GenDrumTone, GenDrumClick, GenDrumSnap
	} GenericDrumInputIds;


	class GenericDrumDevice : public ModuleDevice {
		Value dahrRel_[4];
		Value fre_[6];
		Value fmAmp_;
		Value length_, tune_, tone_, click_, snap_;

		static int setLength(void* context, Value value, void* args);
		static int setTune(void* context, Value value, void* args);
		static int setTone(void* context, Value value, void* args);
		static int setClick(void* context, Value value, void* args);
		static int setSnap(void* context, Value value, void* args);
	public:
		GenericDrumInputs drumInputs;

		GenericDrumDevice(SynthAdapter* adapter, Player* player);
		GenericDrumDevice(SynthAdapter* adapter, Player* player, GenericDrum* drum);
		~GenericDrumDevice();

		void initialize(byte** stream = NULL);

		//InputBase* getInput(int id);
		//void setInput(int id, Value v);

		//int run(int ticks);
		void processCommand(byte cmd, byte*& cursor);

		inline GenericDrum* drum() { return (GenericDrum*)object_; };

		int getPresetBankSize();
		PresetBank* getDefaultPresetBank();

		void assignInputs();

		//static int presetSetter(void*, int, void*);
	};
}
#endif