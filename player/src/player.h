#ifndef __PLAYER_PLAYER_H
#define __PLAYER_PLAYER_H

#include "collection/map.h"
#include "base/stream.h"
#include "player/src/player-defs.h"
#include "player/src/channel.h"
#include "player/src/player-device.h"

NS_FW_BASE_USE

namespace PLAYER {

	class Player {
		static Map* adapters_;
	public:
		static Adapter* addAdapter(Adapter*);
		static Adapter* getAdapter(int type);
		static void cleanUp();			

	#pragma region Resources
	protected:	PROP_R(PArray, channels);
	protected:	PROP_R(PArray, devices);
	protected:	PROP_R(PArray, sequences);
	protected:	PROP_R(PArray, frameLists);
	protected:	PROP_R(Array, dataBlocks);
	protected:	PROP_R(Channel*, masterChannel);
	protected:	PROP(float, refreshRate);
	protected:	PROP_R(DataBlockItem, initData);

	DataBlockItem* addDataBlock(byte* stream, int length, DataBlockItemFlag flag = DataBlockItemFlag::Allocated);
	Sequence* addSequence(Sequence* sequence);
	Sequence* addSequence(byte* stream, int length);
	Device* addDevice(Adapter* adapter, int deviceType, byte** pData = NULL);
	#pragma endregion

	#pragma region Threading
	private: DWORD threadId_;
	private: HANDLE hThread_;
	private: bool isTerminating_;
	private: PROP_R(bool, isPlaying);
	private: static DWORD threadProc(void* lpParameter);
	public:
		void useThread();
		void start();
		void stop();
	#pragma endregion

	#pragma region Player
	private:
		PROP_R(PlayerDevice*, masterDevice);
		PROP_R(int, counter);
	public:
		Player();
		~Player();

		void masterDevice(PlayerDevice* device);
		Channel* addChannel(char* id);
		//void assignChannel(int channelId, Sequence* sequence, int deviceId, int loopCount);
		void initialize(byte** pData = NULL);
		void clear();
		int run(int ticks);
	#pragma endregion
	};
}

#endif