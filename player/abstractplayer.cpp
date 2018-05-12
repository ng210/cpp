#include "abstractplayer.h"
#include "target.h"
#include "base/memory.h"
#include "playeradapter.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

PLAYER_CHANNEL_CREATE* AbstractPlayer::createChannel = NULL;

AbstractPlayer::AbstractPlayer() {
	targets_ = NEW_(PArray);
	adapters_ = NEW_(PArray);
	PlayerAdapter* playerAdapter = NEW_(PlayerAdapter);
	adapters_->add(playerAdapter);
	userData_ = NEW_(Array, sizeof(PLAYER_USER_DATA));
	addTarget(this, playerAdapter);
	sequences_ = NEW_(PArray);
	channels_ = NEW_(PArray);
	masterChannel_ = NULL;
	framesPerSecond_ = 25;
	ticksPerFrame_ = 10;
}
AbstractPlayer::~AbstractPlayer() {
	DEL_(((Target*)targets_->getAt(0))->adapter);
	ARRAY_FOREACH(targets_, DEL_((Target*)value));
	DEL_(targets_);
	ARRAY_FOREACH(channels_, DEL_((AbstractChannel*)value));
	DEL_(channels_);
	DEL_(sequences_);
	DEL_(adapters_);
	DEL_(userData_);
}

void AbstractPlayer::addTarget(void* object, AbstractAdapter* adapter) {
	targets_->add(NEW_(Target, object, adapter));
	adapter->prepareObject(object);
}

//void AbstractPlayer::addSequence(PLAYER_SEQUENCE sequence) {
//	sequences_->add(sequence);
//}
void AbstractPlayer::addSequence(PLAYER_SEQUENCE sequence) {
	sequences_->add(sequence);
}

void AbstractPlayer::addUserData(DWORD length, void* data) {
	PLAYER_USER_DATA userData;
	userData.length = length;
	userData.data = data;
	userData_->add(&userData);
}
//// The very first sequence is assigned to the master channel
	//if (sequences_->length() == 1) {
	//	// The master channel controls the first target
	//	Target* target = (Target*)targets_->getAt(0);
	//	if (channels_->length() == 0) {
	//		masterChannel_ = createChannel(0, target, sequence, Player_Flg_Active);
	//		channels_->add(masterChannel_);
	//	} else {
	//		masterChannel_->assign(this, target, sequence, Player_Flg_Active);
	//	}
	//}

	//// create sequence as Array
	//PArray* sequence = NEW_(PArray);
	//unsigned short delta = 0;
	//int cmd = 0;
	//unsigned char* args = NULL;
	//size_t i = 0;
	//PLAYER_COMMAND_U ptr;
	//ptr.p = (BYTE*)stream;
	//while (true) {
	//	sequence->add(ptr.p);
	//	cmd = ptr.s->code;
	//	if (cmd == (unsigned char)Player_Cmd_end) {
	//		break;
	//	}
	//	ptr.p += offsetof(PLAYER_COMMAND, args) + ptr.s->argc;
	//}
	//sequences_->add(sequence);

	//if (sequences_->length() == 1) {
	//	// the very first sequence is assigned to the master channel
	//	Target* target = (Target*)targets_->getAt(0);
	//	masterChannel_ = NEW_(Channel, this, 0, target, sequence);
	//	masterChannel_->setActive();
	//	masterChannel_->setLooping();
	//	channels_->add(masterChannel_);
	//}

void AbstractPlayer::addChannel(AbstractChannel* chn) {
	if (channels_->length() == 0) {
		masterChannel_ = chn;
	}
	channels_->add(chn);
}

void AbstractPlayer::run(size_t ticks) {
	// run master channel
	if (masterChannel_->isActive()) {
		if (masterChannel_->run(ticks)) {
			// reset
			ARRAY_FOREACH(channels(), ((AbstractChannel*)value)->setActive(false));
		}
	}
	if (masterChannel_->isActive()) {
		// if master is still active run every other channel
		for (UINT32 i = 1; i < channels_->length(); i++) {
			AbstractChannel* chn = (AbstractChannel*)channels_->getAt(i);
			if (chn->isActive()) {
				chn->run(ticks);
			}
		}
	}
}

void AbstractPlayer::updateRefreshRate() {
	refreshRate_ = framesPerSecond_ * ticksPerFrame_;
}

void AbstractPlayer::framesPerSecond(float fps) {
	if (fps > 0.0f) framesPerSecond_ = fps;
	updateRefreshRate();
}
void AbstractPlayer::ticksPerFrame(float tpf) {
	if (tpf > 0.0f) ticksPerFrame_ = tpf;
	updateRefreshRate();
}


NS_PLAYER_END