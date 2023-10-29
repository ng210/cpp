#ifndef __FILE_MGR_H
#define __FILE_MGR_H

#define PLAYER_EDIT_MODE

//#include "synth/ui/synth-app.h"
#include "synth/ui/component.h"
#include "player/src/player-device.h"

using namespace PLAYER;

namespace SYNTH_APP {

	class SynthApp;

	class FileManager : public Component<SynthApp> {
		//PlayerDevice* createBinaryData();	// for tests
	public:
		FileManager(SynthApp* app);
		~FileManager();

		PlayerDevice* loadDefault();
		PlayerDevice* loadBinary(const char* path);
		int saveBinary(const char* path, PlayerDevice* playerDevice);
		PlayerDevice* loadModule(const char* path);
		Map* loadSoundbanks(const char* path);
		int saveSoundbanks(Map* soundbank, const char* path = NULL);
	};
}
#endif