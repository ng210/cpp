#ifndef __SYNTH_ADAPTER_H
#define __SYNTH_ADAPTER_H

#include "AbstractAdapter.h"
#include "Synth.h"
#include "Player.h"

NS_PLAYER_USE

NS_SSN1K_BEGIN

/******************************************************************************
* Commands used by the player adapter
*****************************************************************************/
enum PlayerCommands {
	Synth_Cmd_setNote,
	Synth_Cmd_setControl,

	Synth_Cmd_Count
};

class SynthAdapter : public AbstractAdapter {
	Player* player_;
public:
	SynthAdapter(Player* player);
	virtual ~SynthAdapter();

	int prepareObject(void* object);
	int processCommand(void* target, PLAYER_COMMAND* cmd);
};

NS_SSN1K_END

#endif