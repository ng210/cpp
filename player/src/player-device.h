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

	class Player;
	class Sequence;
	class Channel;
	class PlayerDevice : public Device {
	public:
		static PlayerDevice* create(byte** pData = NULL);

		PlayerDevice(void* object, Adapter* adapter);
		virtual ~PlayerDevice();

		void initialize(byte** pData = NULL);

		bool isActive();
		void isActive(bool b);
		Player* player() { return (Player*)object_; };

		//int run(int ticks);
		void setRefreshRate(float fps);
		void processCommand(byte cmd, byte*& cursor);

#ifdef PLAYER_EDIT_MODE
		void makeCommandImpl(int command, Stream* stream, va_list args);
		int getCommandSize(byte cmd, byte* args);
		int writeToStream(Stream* stream);
#endif
	};
}

#endif