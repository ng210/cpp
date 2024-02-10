#include "synth/ui/song.h"
#include "player/src/player.h"

using namespace SYNTH_APP;

Song::Song(PlayerDevice* playerDevice) {
	name_ = fmw::strdup("untitled");
	tracks_.init(sizeof(Track*), 8);
	dev2frames_.initialize(sizeof(int), MAP_USE_REF, Map::hashingInt, Map::compareInt);
	dev2frames_.hasRefKey(false);
	soundbanks_.initialize(sizeof(int), MAP_USE_REF, Map::hashingInt, Collection::compareInt);

	if (playerDevice == NULL) {
		playerDevice = PlayerDevice::create();
	}

	SynthAdapter sa;
	var synthAdapter = Player::getAdapter(sa.getInfo()->id);
	if (synthAdapter == NULL) {
		synthAdapter = NEW_(SynthAdapter);
		Player::addAdapter(synthAdapter);
	}

	playerDevice_ = playerDevice;
	player_ = playerDevice_->player();
	Key pos = -1;
	mixerDevice_ = (MixerDevice*)player_->devices().search(SynthDevices::DeviceMixer, pos,
		[](COLLECTION_ARGUMENTS) {
			return ((Device*)value)->type() - key.i;
		});
	if (mixerDevice_ == NULL) {
		mixerDevice_ = (MixerDevice*)player_->addDevice(synthAdapter, DeviceMixer);
	}

	var masterSeq = (Sequence*)player_->sequences().get(0);
	if (masterSeq != NULL) {
		// create map deviceId => list of Frames
		masterSeq->reset();
		masterSeq->readByte();	// skip device id
		int time = 0;
		bool isEOS = false;
		while (!isEOS) {
			time += masterSeq->readWord();
			while (true) {
				var cmd = masterSeq->readByte();
				if (cmd != PlayerCommands::CmdAssign) continue;
				var devId = masterSeq->readByte();
				var frames = (PArray*)dev2frames_.get(devId);
				if (!frames) {
					frames = NEW_(PArray);
					dev2frames_.add(devId, frames);
				}
				var frame = frames->search(time, pos, [](COLLECTION_ARGUMENTS) { return key.i - ((Command*)value)->time; });
			}
		}



		trackFrames_
		frames_ = FrameList::fromSequence(masterSeq,
			[](UINT8*& cursor) {
				var channelId = 0;
				var cmdId = *cursor;
				if (cmdId == PlayerCommands::CmdAssign) {

				}
				switch (cmdId) {
				case PlayerCommands::CmdEOS:
				case PlayerCommands::CmdEOF:
					channelId = cmdId;
					break;
				default:
					channelId = cmdId;
				}

				return channelId;
			});
	}
}

Song::~Song() {
	DEL_(playerDevice_);
	// clear soundbanks_?
}
