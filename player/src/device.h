#ifndef __PLAYER_DEVICE_H
#define __PLAYER_DEVICE_H

#include <stdarg.h>
#include "basedef.h"
#include "base/str.h"
#include "base/stream.h"
#include "player/src/player-defs.h"

NS_FW_BASE_USE

namespace PLAYER {

	class Adapter;
	class Player;

	class Device {
	protected: PROP_R(float, refreshRate);
	protected: void* object_;
	protected: PROP(int, type);
	protected: PROP(Adapter*, adapter);
	protected: PROP(Player*, player);
	protected: PROP(byte*, initData);
	protected: bool isActive_;
	public:
		Device(void* object, Adapter* adapter);
		virtual ~Device() {};

		virtual void initialize(byte** pData = NULL) = 0;

		virtual bool isActive();
		virtual void isActive(bool b);

		virtual int run(int ticks);
		virtual void setRefreshRate(float fps);
		virtual void processCommand(byte cmd, byte*& cursor) = 0;

		void* object() { return object_; }
		void object(void* obj) { object_ = obj; }

#ifdef PLAYER_EDIT_MODE
		//virtual byte* makeCommand(byte command, ...);
		virtual Stream* makeCommand(byte command, ...);
		virtual void makeCommandImpl(int command, Stream* stream, va_list args) = 0;
		virtual int getCommandSize(byte cmd, byte* args) = 0;
		virtual int writeToStream(Stream* stream);
#endif
	};
}
#endif