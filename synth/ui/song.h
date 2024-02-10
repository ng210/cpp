#ifndef __SONG_H
#define __SONG_H

#include "win/ctrl.h"
#include "synth/ui/song-ctrl.h"
#include "player/src/player-device.h"
#include "synth/src/device/mixer-device.h"

using namespace SYNTH;
using namespace PLAYER;

namespace SYNTH_APP {
	class Song {
		PROP_R(char*, name);
		PROP_R(Array, tracks);
		Map dev2frames_;
		PROP(Map, soundbanks);

		PROP_R(PlayerDevice*, playerDevice);
		PROP_R(Player*, player);
		PROP_R(MixerDevice*, mixerDevice);

		PROP_R(SongCtrl*, control);

	public:
		Song(PlayerDevice* playerDevice = NULL);
		virtual ~Song();
	};
}
#endif


