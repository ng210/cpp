#ifndef __SYNTH_ADAPTER_H
#define __SYNTH_ADAPTER_H

#include "abstractadapter.h"
#include "synth.h"

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

class SynthAdapter : public AbstractAdapter {
	Player* player_;
public:
	SynthAdapter(Player* player);
	virtual ~SynthAdapter();

	int prepareObject(void* object);
	int processCommand(void* object, PLAYER_COMMAND command);
	void setTempo(void *object, float ticksPerSecond);
	PLAYER_COMMAND createCommand(int code, ...);
#ifdef _DEBUG
	char* logCommand(PLAYER_COMMAND command);
#endif
	int dumpCommand(PLAYER_COMMAND cmd, Buffer* buffer);
};

NS_SSN1K_END

#endif