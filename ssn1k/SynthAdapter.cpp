#include "SynthAdapter.h"
#include "Player.h"

NS_SSN1K_BEGIN

SynthAdapter::SynthAdapter(Player* player) {
	player_ = player;
}

SynthAdapter::~SynthAdapter() {
}

int SynthAdapter::prepareObject(void* object) {
	return 0;
}
int SynthAdapter::processCommand(void* target, PLAYER_COMMAND* command) {
	Synth* synth = (Synth*)target;
	unsigned char* args = &command->args;
	int duration = 0;
	switch (command->code) {
		case Synth_Cmd_setNote:
			duration = args[1] * (int)(SSN1K::getSampleRate() / player_->refreshRate());
			synth->setNote(args[0], duration, args[2]/127.0f);
			break;
		case Synth_Cmd_setControl:
			int ctrlId = args[0];
			synth->getControl(ctrlId)->set(args[1] / 127.0f);
			break;
	}
	return 1;
}

NS_SSN1K_END