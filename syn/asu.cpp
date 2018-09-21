#include "soundlib/soundplayer.h"
#include "asu.h"
#include "xmloader.h"
#include "base/string.h"
#include "syn/synsequence.h"

NS_FW_BASE_USE
NS_PLAYER_USE
NS_SYN_USE

Map Asu::adapterMap_(sizeof(UINT32), sizeof(ADAPTER_INFO), Map::hashingItem, Collection::compareInt);
UINT32 Asu::sampleRate_ = 48000;
PArray Asu::playBackQueue_;
bool Asu::isPlaying_ = false;
bool Asu::isCallbackRunning_ = false;

// static methods
void Asu::registerAdapter(ADAPTER_INFO* info) {
	Asu::adapterMap_.put(&info->id, info);
}
const ADAPTER_INFO* Asu::getAdapterInfo(UINT32 id) {
	return (const ADAPTER_INFO*)Asu::adapterMap_.get(&id);
}
Array* const Asu::getAdapterInfoList() {
	Array* arr = NEW_(Array, sizeof(ADAPTER_INFO));
	MAP_FOREACH(&adapterMap_, arr->add(value););
	return arr;
}


Asu::Asu() : adapters_(sizeof(ADAPTER_DATA)), userDataBlockMap_(sizeof(int), sizeof(USER_DATA_BLOCK), Map::hashingItem, Collection::compareInt) {
	player_ = NULL;
	frameCounter_ = 0;
	frames_ = 0;
}
Asu::~Asu() {
	// remove from queue
	while (Asu::isCallbackRunning_);
	ARRAY_FOREACH(&Asu::playBackQueue_, if (value == (void*)this) { Asu::playBackQueue_.removeAt(i); break; });
	if (player_ != NULL) {
		// remove sequences
		ARRAY_FOREACH(player_->sequences(), DEL_((Sequence*)value););
		// remove user data blocks
		ARRAY_FOREACH(player_->userDataBlocks(), FREE(value););
		// remove player
		DEL_(player_);
	}
}

ADAPTER_DATA* Asu::addAdapter(int id, int udbId) {
	ADAPTER_INFO* adapterInfo = (ADAPTER_INFO*)adapterMap_.get(&id);
	ADAPTER_DATA* data = NULL;
	if (adapterInfo != NULL) {
		ADAPTER_DATA adapterData = { adapterInfo, udbId };
		data = (ADAPTER_DATA*)adapters_.add(&adapterData);
	}
	return data;
}
int Asu::addSequence(Sequence* sequence) {
	return sequences_.add(sequence) != NULL ? 1 : 0;
}
int Asu::addSequences(PArray* sequences) {
	int count = sequences_.length();
	ARRAY_FOREACH(sequences, addSequence((Sequence*)value); );
	return sequences_.length() - count;
}
UINT32 Asu::addUserDataBlock(UINT32 size, UINT8* data) {
	// use map to reference data blocks index-independenly
	UINT32 udbId = (UINT32)userDataBlockMap_.size();
	USER_DATA_BLOCK udb = { size, data };
	userDataBlockMap_.put(&udbId, &udb);
	return udbId;
}

int Asu::prepare() {
	DEL_(player_);
	player_ = NEW_(Player);
	// add user data blocks
	MAP_FOREACH((&userDataBlockMap_), player_->addUserDataBlock(((USER_DATA_BLOCK*)value)->size, ((USER_DATA_BLOCK*)value)->data) );

	// initialize adapters, add targets
	for (UINT32 ai = 0; ai < adapters_.length(); ai++) {
		ADAPTER_DATA* adapterData = (ADAPTER_DATA*)adapters_.getAt(ai);
		USER_DATA_BLOCK* udb = (USER_DATA_BLOCK*)userDataBlockMap_.get(&adapterData->udbId);
		UINT8* data = udb->data;
		IAdapter* adapter = adapterData->info->create(&data);
		//adapter->createTargets(player_);
		if (adapterData->info->id == SynthAdapter::adapterInfo().id) {
			soundAdapter_ = adapter;
		}
	}

	// add sequences
	for (UINT32 si=0; si<sequences_.length(); si++) {
		player_->addSequence(sequences_.getAt(si));
	}

	return 0;
}
void Asu::run(UINT32 startFrame) {
	//// fast forward to start
	//player_->run(startFrame);
	// add this asu to playback
	Asu::playBackQueue_.add(this);
	if (!Asu::isPlaying_) {
		// set up playback
		if (SoundPlayer::start(Asu::sampleRate_, 1, Asu::soundCallback) != 0) {
			// error
		} else {
			Asu::isPlaying_ = true;
		}
	}
}

char* Asu::info() {
	PArray stringBuilder;
	stringBuilder.add(NS_FW_BASE::strdup("Adapters"));
	for (UINT32 i = 0; i < adapters_.length(); i++) {
		const ADAPTER_INFO* adapterInfo = ((ADAPTER_DATA*)adapters_.getAt(i))->info;
		stringBuilder.add(str_format("- %04X (%s)", adapterInfo->id, &adapterInfo->id));
	}
	stringBuilder.add(NS_FW_BASE::strdup("\nSequences"));
	stringBuilder.add("master sequence");
	IAdapter* playerAdapter = NEW_(PlayerAdapter);
	IAdapter* synthAdapter = NEW_(SynthAdapter);
	stringBuilder.add(((SynSequence*)sequences_.getAt(0))->info(playerAdapter));
	stringBuilder.add(NS_FW_BASE::strdup(""));
	DEL_(playerAdapter);
	for (UINT32 i = 1; i < sequences_.length(); i++) {
		stringBuilder.add(str_format("#%2d. sequence", i));
		stringBuilder.add(((SynSequence*)sequences_.getAt(i))->info(synthAdapter));
		stringBuilder.add(NS_FW_BASE::strdup(""));
	}
	DEL_(synthAdapter);
	stringBuilder.add(NS_FW_BASE::strdup("\n"));
	char* text = stringBuilder.str_join("\n");
	ARRAY_FOREACH(&stringBuilder, FREE(value); );
	return text;
}

bool Asu::isActive() {
	return player_->masterChannel()->isActive();
}

int Asu::importFromXm(const char* path, int& start) {
	PlayerAdapter* playerAdapter = NEW_(PlayerAdapter);
	SynthAdapter* synthAdapter = NEW_(SynthAdapter);
	XmLoader xmLoader(playerAdapter, synthAdapter);
	int ret = 0;
	if (!xmLoader.load(path)) {
		// todo
		PArray* sequences = xmLoader.convert();
		ret = sequences->length();
		addSequences(sequences);
		DEL_(sequences);
	} else {
		start = -1;
	}
	DEL_(synthAdapter);
	DEL_(playerAdapter);

	return ret;
}

void Asu::soundCallback(short* buffer, int sampleCount) {
	Asu::isCallbackRunning_ = true;
	for (UINT32 i = 0; i < Asu::playBackQueue_.length(); i++) {
		Asu* asu = (Asu*)Asu::playBackQueue_.getAt(i);
		size_t start = 0;
		size_t end = 0;
		size_t remains = sampleCount;
		while (remains) {
			if (asu->frameCounter_ == 0) {
				asu->player_->run(1);
				asu->frameCounter_ = (size_t)(Asu::sampleRate_ / asu->player_->refreshRate());
				asu->frames_++;
			}
			size_t len = asu->frameCounter_ < remains ? asu->frameCounter_ : remains;
			end = start + len;
			asu->frameCounter_ -= (UINT32)len;
			asu->soundAdapter_->fill(buffer, start, end);
			start = end;
			remains -= len;
		}
	}
	Asu::isCallbackRunning_ = false;
}

//// this.player.sequences[0].run(startFrame);
//if (SoundPlayer::start(Asu::sampleRate_, 1, soundCallback) == 0) {
//
//	while (player_->masterChannel()->isActive()) {
//		Sleep(10);
//	}
//	printf("\n");
//	printf("Stopping audio playback\n");
//	SoundPlayer::stop();
//}

//void Asu::addAdapter(IAdapter* adapter, size_t length, void* userDataBlock) {
//	addUserDataBlock(length, userDataBlock);
//	player_->addAdapter(adapter);
//}
//void Asu::addSequence(Sequence* sequence) {
//	player_->addSequence(sequence);
//}
//void Asu::addSequences(PArray* sequences) {
//	ARRAY_FOREACH(player_->sequences(), player_->addSequence((Sequence*)value););
//}
//void Asu::addUserDataBlock(size_t length, void* userDataBlock) {
//	player_->addUserDataBlock(length, userDataBlock);
//}
//
//int Asu::exportToFile(const char* path) {
//	// export to a new .asu file
//	return 0;
//}
//
//int Asu::run(int ticks) {
//	player_->run(ticks);
//	return 0;
//}
//
//int Asu::pause() {
//	return 0;
//}
//
//int Asu::seek(int ticks) {
//	player_->run(ticks);
//	return player_->masterChannel()->isActive();
//}