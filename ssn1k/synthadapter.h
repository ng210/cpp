#ifndef __SYNTH_ADAPTER_H
#define __SYNTH_ADAPTER_H

#include "iadapter.h"
#include "synth.h"
#include "mixer.h"

NS_PLAYER_USE
NS_SSN1K_BEGIN

#define SYNTH_ADAPTER_ID 0x10

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

typedef struct SYNTH_CMD_SET_NOTE_ON_ : ABSTRACT_ADAPTER_COMMAND_ {
	UINT8 note;
	UINT8 velocity;
} SYNTH_CMD_SET_NOTE_ON;

typedef struct SYNTH_CMD_SET_NOTE_OFF_ : ABSTRACT_ADAPTER_COMMAND_ {
	UINT8 note;
} SYNTH_CMD_SET_NOTE_OFF;

typedef struct SYNTH_CMD_SET_CTRL_ : ABSTRACT_ADAPTER_COMMAND_ {
	UINT8 id;
	UINT8 value;
} SYNTH_CMD_SET_CTRL;

typedef struct SYNTH_CMD_SET_CTRLF_ : ABSTRACT_ADAPTER_COMMAND_ {
	UINT8 id;
	float value;
} SYNTH_CMD_SET_CTRLF;

typedef struct SYNTH_CMD_PRG_CHNG_ : ABSTRACT_ADAPTER_COMMAND_ {
	UINT8 id;
} SYNTH_CMD_PRG_CHNG;

typedef struct SYNTH_CREATE_TARGET_ {
	UINT8 voiceCount;
	SYNTH_BANK* bank;
} SYNTH_CREATE_TARGET;

class SynthAdapter : public IAdapter {
	Mixer* mixer_;
	UINT8 synthCount_;
	Synth* synths_[16];

	static int createBank(SYNTH_BANK* bankConfig, out Ctrl*& bank);
public:
	SynthAdapter();
	virtual ~SynthAdapter();

	int initialize(void* data, Player* player);
	int getId();
	int processCommand(void* object, PLAYER_COMMAND command);
	void setTempo(void *object, float ticksPerSecond);
	Target* createTarget(void* initData);
	PLAYER_COMMAND createCommand(int code, ...);
#ifdef _DEBUG
	char* logCommand(PLAYER_COMMAND command);
#endif
	int dumpCommand(PLAYER_COMMAND cmd, Buffer* buffer);
	int matchCommand(int filter, PLAYER_COMMAND cmd);
	int getCommandParameters(PLAYER_COMMAND cmd, double* parameters);

	void run(void* buffer, size_t start, size_t end);
};

NS_SSN1K_END

#endif