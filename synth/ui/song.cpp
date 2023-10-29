#include "synth/ui/song.h"
#include "player/src/player.h"

using namespace SYNTH_APP;

Song::Song(PlayerDevice* playerDevice, Window* parent) {
	name_ = fmw::strdup("untitled");
	playerDevice_ = playerDevice;
	player_ = playerDevice_->player();
	Key pos = -1;
	mixerDevice_ = (MixerDevice*)player_->devices().search(SynthDevices::DeviceMixer, pos,
		[](COLLECTION_ARGUMENTS) {
			return ((Device*)value)->type() - key.i;
		});
	control_ = NEW_(SongCtrl);
	control_->create(parent, this);
	soundbanks_.initialize(sizeof(int), MAP_USE_REF, Map::hashingInt, Collection::compareInt);
}

Song::~Song() {
	DEL_(control_);
	DEL_(playerDevice_);
	// clear soundbanks_?
}
