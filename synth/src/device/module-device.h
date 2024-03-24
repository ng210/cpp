#ifndef __MODULE_DEVICE_H
#define __MODULE_DEVICE_H

#include "player/src/device.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/module/module.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {

	#pragma region Defines
	typedef struct AdsrInputs_ {
		InputF amp;
		Input atk;
		Input dec;
		InputF8 sus;
		Input rel;
	} AdsrInputs;
	#define AdsrInputsSize 8	// 4 + 4*1 

	typedef struct DahrInputs_ {
		InputF amp;
		Input del;
		Input atk;
		Input hld;
		Input rel;
	} DahrInputs;
	#define DahrInputsSize 8	// 4 + 4*1 

	typedef struct OscInputs_ {
		InputF8 amp;
		InputF fre;
		Input note;
		Input tune;
		InputF8 psw;
		Input wave;
	} OscInputs;
	#define OscInputsSize 9	// 4 + 5*1 

	typedef struct LfoInputs_ {
		InputF amp;
		InputF fre;
	} LfoInputs;
	#define LfoInputsSize 8	// 2*4

	typedef struct FltInputs_ {
		Input cut;
		InputF8 res;
		Input mode;
	} FltInputs;
	#define FltInputsSize 3

	typedef struct DlyInputs_ {
		InputF8 feedback;
		InputF delay;
	} DlyInputs;
	#define DlyInputsSize 5	// 4 + 1 

	typedef struct ClpInputs_ {
		InputF amp;
		InputF8 lvl;
	} ClpInputs;
	#define ClpInputsSize 5	// 4 + 1 

	typedef enum ModuleCommands {
		CmdSetUint8		= 2,	// ctrlId, value
		CmdSetFloat8	= 3,	// ctrlId, value
		CmdSetFloat 	= 4,	// ctrlId, value
		CmdSetNote		= 10,	// note, velocity
		CmdSetVelocity	= 11,	// velocity
		CmdSetProgram	= 12	// programId
	} ModuleCommands;
	#pragma endregion

	class ModuleDevice : public Device {
	protected: PROP(int, datablockId);
	public:
		ModuleDevice(Adapter* adapter, Player* player, void* object);

		inline Module* module() { return (Module*)object_; }

		void initialize(byte** stream = NULL);

		bool isActive();
		//void isActive(bool b);

		int run(int ticks);
		//void setRefreshRate(float fps);

		void processCommand(byte cmd, byte*& cursor);

		//virtual PresetBank* createSoundbank();
		virtual int getPresetBankSize() = 0;
		virtual PresetBank* getDefaultPresetBank() = 0;

		virtual void assignInputs();
		
		//static byte* loadSoundbanks(const char* soundbankPath);

		static COLLECTION_COMPARE compareToModule;
		static Map* loadPresetBanks(const char* presetBankPath, SynthAdapter* synthAdapter);

		static void setupAdsr(AdsrInputs*);
		static void setupDahr(DahrInputs*);
		static void setupOsc(OscInputs*);
		static void setupLfo(LfoInputs*);
		static void setupFlt(FltInputs*);
		static void setupClp(ClpInputs*);
		static void setupDly(DlyInputs*);
	};
}
#endif