#ifndef __PLAYER_PLAYER_H
#define __PLAYER_PLAYER_H

#include "collection/map.h"
#include "channel.h"

NS_FW_BASE_USE

namespace PLAYER {

	typedef enum PlayerCommands {
		CmdEOF = 0,
		CmdEOS = 1,
		CmdAssign = 2, // Parameters: channel id, sequence id, device id, loop count
		CmdTempo = 3
	} PlayerCommands;

	typedef enum PlayerDevices {
		DevPlayer = 0,
		DevChannel = 1
	} PlayerDevices;

	class Player;

	typedef void* (PlayerEnumerator)(Player*);

	typedef struct AdapterDataItem {
		Adapter* adapter;
		int userDataBlock;
		AdapterDataItem(Adapter* a, int d) : adapter(a), userDataBlock(d) {}
	} AdapterDataItem;

	typedef struct UserDataBlockItem {
		int length;
		byte* userDataBlock;
		UserDataBlockItem(int l, byte* data) : length(l), userDataBlock(data) {}
	} UserDataBlockItem;

	typedef struct PlayerBinaryHeader {
		word headerSize;
		word adapterTableOffset;
		word sequenceTableOffset;
		word dataBlockTableOffset;
	} PlayerBinaryHeader;

	class Player : public Adapter, public Device {
	protected:	PROP_R(Array, adapters);
	protected:	PROP_R(PArray, sequences);
	protected:	PROP_R(Array, userDataBlocks);
	protected:	PROP_R(PArray, channels);
	protected:	PROP_R(Channel*, masterChannel);
	protected:	PROP_R(float, refreshRate);
	
	public:
		Player();
		~Player();

        void initialize(void* args);

        // Adapter implementation
		AdapterInfo* info();
        Device* createDeviceImpl(int deviceType, byte** initData);
        byte* processCommand(Channel* channel, byte command);
        void setRefreshRate(float fps);

        // Player methods
        int load(const char* path);
        int load(byte* stream);

		bool run(int ticks);
		void reset();
		inline bool isActive();

		Adapter* addAdapter(int adapterType, int datablockId);
		UserDataBlockItem* addDatablock(byte* stream, int length);
		int createStream(byte*& stream, Array* adapters = NULL, PArray* sequences = NULL, Array* userDataBlocks = NULL);
		size_t save(const char* path);
		Sequence* createSequence(byte* stream, int length);
		byte* makeCommand(byte command, Sequence* sequence, byte* cursor);

        // static members
	private:
		static Map adapterTypes_;
	public:
		static AdapterInfo Info;

		static void registerAdapter(AdapterInfo& info);
        static AdapterInfo* getAdapterInfoByName(const char* name);
		
		static Map& adapterTypes();
        static Adapter* creator(Player* player);
		static void initializer(void* args);
	};
}

#endif