#ifndef __TRACK_H
#define __TRACK_H

#include "player/src/device.h"
#include "player/src/frame-list.h"

using namespace PLAYER;
namespace SYNTH_APP {

	class Track {
		PROP_R(Device*, device);
		PROP_R(FrameList*, frames);

	public:
		Track(Device* device, FrameList* frames);
	};
}
#endif