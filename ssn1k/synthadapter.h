#ifndef __SYNTH_ADAPTER_H
#define __SYNTH_ADAPTER_H

//#include <windows.h>

#include "iadapter.h"
#include "synth.h"
#include "mixer.h"

NS_PLAYER_USE
NS_SSN1K_BEGIN

#define SYNTH_CREATE_TARGET_DIALOG 0x1000

/******************************************************************************
* Commands used by the player adapter
*****************************************************************************/
enum PlayerCommands : UINT8 {
	Synth_Cmd_setNoteOn,	// note on with velocity
	Synth_Cmd_setNoteOff,	// note off
	Synth_Cmd_setControl,	// set control from UINT8 / 255
	Synth_Cmd_setControlB,	// set control from UINT8 as float
	Synth_Cmd_setControlF,	// set control from float
	Synth_Cmd_prgChange,	// change program

	Synth_Cmd_Count
};

typedef struct SYNTH_CMD_SET_NOTE_ON_ : ADAPTER_COMMAND_BASE {
	UINT8 note;
	UINT8 velocity;
} SYNTH_CMD_SET_NOTE_ON;

typedef struct SYNTH_CMD_SET_NOTE_OFF_ : ADAPTER_COMMAND_BASE {
	UINT8 note;
} SYNTH_CMD_SET_NOTE_OFF;

typedef struct SYNTH_CMD_SET_CTRL_ : ADAPTER_COMMAND_BASE {
	UINT8 ctrlId;
	UINT8 value;
} SYNTH_CMD_SET_CTRL;

typedef struct SYNTH_CMD_SET_CTRLF_ : ADAPTER_COMMAND_BASE {
	UINT8 ctrlId;
	float value;
} SYNTH_CMD_SET_CTRLF;

typedef struct SYNTH_CMD_PRG_CHNG_ : ADAPTER_COMMAND_BASE {
	UINT8 prgId;
} SYNTH_CMD_PRG_CHNG;

enum SYNTHADAPTER_TARGET {
	POLYSYNTH01,
	MONOSYNTH01
};

typedef struct SYNTH_CREATE_GENERIC_ {
	UINT8 type;
} SYNTH_CREATE_GENERIC;

typedef struct SYNTH_CREATE_SYNTH_ : SYNTH_CREATE_GENERIC {
	UINT8 voiceCount;
	UINT8 bankId;
} SYNTH_CREATE_SYNTH;

typedef struct SYNTH_CREATE_EFFECT_ : SYNTH_CREATE_GENERIC {
} SYNTH_CREATE_EFFECT;

typedef struct SYNTH_CREATE_OBJECTS_ {
	UINT8 count;
	SYNTH_CREATE_GENERIC* objects;
} SYNTH_CREATE_OBJECTS;

typedef struct SYNTH_PREPARE_CONTEXT_ {
	UINT8 bankCount;
	UINT8* banks;
} SYNTH_PREPARE_CONTEXT;



class SynthAdapter : public IAdapter {
	Mixer* mixer_;
	PROP_R(PArray*, banks);
	//PROP_R(PArray*, synths);


	static const ADAPTER_INFO adapterInfo_;
	static SynthAdapter instance_;

	static int createBank(SYNTH_BANK* bankConfig, out Ctrl*& bank);
public:
	SynthAdapter();
	virtual ~SynthAdapter();

	void createObjects(PArray* targets, void* data);
	const ADAPTER_INFO& getInfo();
	static const IAdapter& getInstance();
	void prepareContext(void* data);
	int processCommand(void* target, PLAYER_COMMAND_U data);
	void updateRefreshRate(void* target, float ticksPerSecond);

	//size_t fill(void* buffer, size_t start, size_t end);

#ifdef _EDITOR
	// editor extensions
	virtual PLAYER_COMMAND_U createCommand(int id, ...) = 0;
	int getCommandParameters(PLAYER_COMMAND cmd, double* parameters);
	int matchCommand(int filter, PLAYER_COMMAND cmd);
	char* logCommand(PLAYER_COMMAND command);
	int dumpCommand(PLAYER_COMMAND command, Buffer* buffer);
	Target* createTarget(int id, UINT8* data);
#endif

};

NS_SSN1K_END

#endif