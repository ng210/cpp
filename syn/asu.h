#ifndef __ASU_H
#define __ASU_H

#include "multichartview/multichartview.h"
#include "player/player.h"
#include "seqseries.h"

NS_FW_BASE_USE
NS_PLAYER_USE

class Asu {
protected: PROP_R(Player*, player);

public:
	Asu();
	Asu(const char* path);

	~Asu();

// construction
	void addAdapter(IAdapter* adapter, size_t length = 0, void* initData = NULL);
	//void addAdapter(int* adapterId, size_t length = 0, void* initData = NULL);
	void addSequence(Sequence* sequence);
	void addSequences(PArray* sequences);
	void addUserDataBlock(size_t length, void* userDataBlock);

// import/export
	int importXm(const char* path, int& start);
	int exportToFile(const char* path);

// playback
	int run(int ticks);
	int pause();
	int seek(int frame);
};

#endif