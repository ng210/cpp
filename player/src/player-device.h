#ifndef __PLAYER_PLAYER_DEVICE_H
#define __PLAYER_PLAYER_DEVICE_H

#include "collection/parray.h"
#include "collection/map.h"
#include "player/src/device.h"

NS_FW_BASE_USE

namespace PLAYER {

	typedef enum PlayerCommands {
		CmdEOF = 0,
		CmdEOS = 1,
		CmdAssign = 2, // Parameters: channel id, sequence id, device id, loop count
		CmdTempo = 3
	} PlayerCommands;

	typedef enum PlayerMode {
		PlayerModeSequence,
		PlayerModeFrameList
	} PlayerMode;

	class Player;
	class Sequence;
	class Channel;
	class PlayerDevice : public Device {
	public:
		PROP(PlayerMode, mode);

		PlayerDevice(Adapter* adapter, Player* player, void* object);
		virtual ~PlayerDevice();

		void initialize(byte** pData = NULL);

		bool isActive();
		void isActive(bool b);

		void setValue(int id, Value value);
		Value* getValue(int id);

		int run(int ticks);

		Player* player() { return (Player*)object_; };

		void processCommand(byte cmd, byte*& cursor);

		static PlayerDevice* create(byte** pData = NULL);
	};
}

#endif