#ifndef __PLAYER_SEQUENCE_H
#define __PLAYER_SEQUENCE_H

#include "collection/parray.h"
#include "stream.h"
#include "adapter.h"
#include "player.h"

NS_FW_BASE_USE
namespace PLAYER {
	class Sequence : public Stream {
	private: PROP_R(Adapter*, adapter);
	private: PROP_R(PArray, frames);
	public:
		Sequence();
		Sequence(Adapter* adapter);
		Sequence(Adapter* adapter, byte* stream, int offset = 0, int length = 0);
		virtual ~Sequence();

		void init(int length);

		//byte getByte(byte*& cursor);
		//word getWord(byte*& cursor);
		//dword getDword(byte*& cursor);
		//float getFloat(byte*& cursor);
		//char* getString(byte*& cursor);

		Sequence* writeHeader();
		Sequence* writeDelta(word delta);
		Sequence* writeCommand(byte cmd);
		Sequence* writeEOF();
		Sequence* writeEOS();
		//Sequence* writeByte(byte value);
		//Sequence* writeString(char* str);
		//Sequence* writeStream(byte* stream, int length);
		//Sequence* writeWord(word value);
		//Sequence* writeDword(dword value);
		//Sequence* writeFloat(float value);

		PArray* toFrames();
		static Sequence* fromFrames(Collection* frames, Adapter* adapter);
	};
}

#endif