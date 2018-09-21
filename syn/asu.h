#ifndef __ASU_H
#define __ASU_H

//#include "multichartview/multichartview.h"
#include "basedef.h"
#include "player/player.h"
#include "player/sequence.h"

#include "collection/map.h"	// SYN-EDITOR

NS_FW_BASE_USE
NS_PLAYER_USE

typedef struct USER_DATA_BLOCK_ {
	UINT32 size;
	UINT8* data;
} USER_DATA_BLOCK;


class Asu {
protected:
	// Map<int, AdapterInfo>
	static Map adapterMap_;	// SYN-EDITOR
	// List<AdapterData>
	Array adapters_;
	// List<Sequence>
	PArray sequences_;
	// Map<int, UserDataBlock>
	Map userDataBlockMap_;
	// dedicated adapter for the playback
	IAdapter* playbackAdapter_;
	// dedicated adapter for the sound playback
	IAdapter* soundAdapter_;
	// playback queue of asu instances
	static PArray playBackQueue_;
	static bool isPlaying_;
	UINT32 frameCounter_;
	UINT32 frames_;

protected: PROP_R(Player*, player);
protected:
	static UINT32 sampleRate_;
public:
	static UINT32 sampleRate() { return sampleRate_; }

public:
	Asu();
	~Asu();

	static void registerAdapter(ADAPTER_INFO* info);
	static const ADAPTER_INFO* getAdapterInfo(UINT32 id);
	static Array* const getAdapterInfoList();
	
	static void soundCallback(short* buffer, int sampleCount);
	static bool isCallbackRunning_;

	ADAPTER_DATA* addAdapter(int id, int udbId);
	int addSequence(Sequence* sequence);
	int addSequences(PArray* sequences);
	UINT32 addUserDataBlock(UINT32 size, UINT8* data);
	int importFromXm(const char* path, int& start);
	int prepare();
	void run(UINT32 startFrame = 0);
	bool isActive();

	char* info();
};

//// construction
//	void addAdapter(IAdapter* adapter, size_t length = 0, void* initData = NULL);
//	//void addAdapter(int* adapterId, size_t length = 0, void* initData = NULL);
//	void addSequence(Sequence* sequence);
//	void addSequences(PArray* sequences);
//	void addUserDataBlock(size_t length, void* userDataBlock);
//
//// import/export
//	int importXm(const char* path, int& start);
//	int exportToFile(const char* path);
//
//// playback
//	int run(int ticks);
//	int pause();
//	int seek(int frame);
//};

#endif