#include "asu.h"
#include "xmloader.h"

NS_FW_BASE_USE
NS_PLAYER_USE

Asu::Asu() {
	player_ = NEW_(Player);
}

Asu::Asu(const char* path) {
	// load .asu file
}

Asu::~Asu() {
	// remove sequencs
	ARRAY_FOREACH(player_->sequences(), DEL_((Sequence*)value););
	// remove user data blocks
	ARRAY_FOREACH(player_->userDataBlocks(), FREE(value););
	// remove player
	DEL_(player_);
}

void Asu::addAdapter(IAdapter* adapter, size_t length, void* userDataBlock) {
	player_->addAdapter(adapter);
	addUserDataBlock(length, userDataBlock);
}
void Asu::addSequence(Sequence* sequence) {
	player_->addSequence(sequence);
}
void Asu::addSequences(PArray* sequences) {
	ARRAY_FOREACH(player_->sequences(), player_->addSequence((Sequence*)value););
}
void Asu::addUserDataBlock(size_t length, void* userDataBlock) {
	player_->addUserDataBlock(length, userDataBlock);
}

int Asu::importXm(const char* path, int& start) {
	PlayerAdapter* playerAdapter = NEW_(PlayerAdapter);
	SynthAdapter* synthAdapter = NEW_(SynthAdapter);
	XmLoader xmLoader((PlayerAdapter*)player_->adapters()->getAt(0), synthAdapter);
	xmLoader.load(path);
	PArray* sequences = xmLoader.convert();
	int startIndex = sequences->length();
	addSequences(sequences);
	DEL_(sequences);
	DEL_(synthAdapter);
	DEL_(playerAdapter);
	start = player_->sequences()->length();
	return startIndex;
}

int Asu::exportToFile(const char* path) {
	// export to a new .asu file
	return 0;
}

int Asu::run(int ticks) {
	player_->run(ticks);
	return 0;
}

int Asu::pause() {
	return 0;
}

int Asu::seek(int ticks) {
	player_->run(ticks);
	return player_->masterChannel()->isActive();
}