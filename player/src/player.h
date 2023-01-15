#ifndef __PLAYER_PLAYER_H
#define __PLAYER_PLAYER_H

#include "collection/map.h"
#include "base/stream.h"
#include "player-device.h"

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
	protected:	PROP_R(Array, dataBlocks);
	protected:	PROP_R(Channel*, masterChannel);
	protected:	PROP_R(float, refreshRate);
	protected:	PROP_R(DataBlockItem, initData);

	DataBlockItem* addDataBlock(byte* stream, int length, DataBlockItemFlag flag = DataBlockItemFlag::Allocated);
	Sequence* addSequence(Sequence* sequence);
	Sequence* addSequence(byte* stream, int length);
	Device* addDevice(Adapter* adapter, int deviceType, byte** pData = NULL);
#pragma endregion

#pragma region Threading
	private:
		DWORD threadId_;
		HANDLE hThread_;
		bool isTerminating_;
		bool isPlaying_;
		static DWORD threadProc(void* lpParameter);
	public:
		void useThread();
		void start();
		void stop();
#pragma endregion

#pragma region Player
	private:
		PROP_R(PlayerDevice*, masterDevice);
	public:
		Player();
		~Player();

		void masterDevice(PlayerDevice* device);
		void assignChannel(int channelId, Sequence* sequence, int deviceId, int loopCount);
		void initialize(byte** pData);
		void clear();
		int run(int ticks);

		void load(byte** pData);
		Stream* save();

#pragma endregion

	};
}

#endif