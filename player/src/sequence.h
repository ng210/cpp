#ifndef __PLAYER_SEQUENCE_H
#define __PLAYER_SEQUENCE_H

#include "collection/parray.h"
#include "stream.h"
#include "frame.h"

NS_FW_BASE_USE
namespace PLAYER {
	class Device;

	class Sequence : public Stream {
	private: PROP_R(Device*, device);
	private: PROP_R(PArray*, frames);
	public:
		Sequence();
		Sequence(Device* device);
		Sequence(Device* device, byte* stream, int offset = 0, int length = 0);
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
		Sequence* writeByte(byte value);
		Sequence* writeString(char* str);
		Sequence* writeStream(byte* stream, int length);
		Sequence* writeWord(word value);
		Sequence* writeDword(dword value);
		Sequence* writeFloat(float value);

		PArray* toFrames(Device* device = NULL);
		static Sequence* fromFrames(Collection* frames, Device* device);
		char* print(Device* device = NULL);
	};
}

#endif