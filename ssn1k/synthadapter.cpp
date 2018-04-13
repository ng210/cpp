#include "synthadapter.h"
#include "player.h"

NS_SSN1K_BEGIN

SynthAdapter::SynthAdapter(Player* player) {
	player_ = player;
}

SynthAdapter::~SynthAdapter() {
}

int SynthAdapter::prepareObject(void* synth) {
	return 0;
}
int SynthAdapter::processCommand(void* object, PLAYER_COMMAND* command) {
	Synth* synth = (Synth*)object;
	unsigned char* args = &command->args;
	int duration = 0;
	switch (command->code) {
		case Synth_Cmd_setNote:
			duration = args[1] * (int)(SSN1K::getSampleRate() / player_->refreshRate());
			synth->setNote(args[0], duration, args[2]/127.0f);
			break;
		case Synth_Cmd_setControl:
			int ctrlId = args[0];
			if (ctrlId != SSN1K_CI_Tune) {
				synth->getControl(ctrlId)->set(args[1]/127.0f);
			} else {
				//printf("Tune: %d\n", args[1]);
				synth->getControl(ctrlId)->set((float)args[1], false);
			}
			break;
	}
	return 1;
}

NS_SSN1K_END