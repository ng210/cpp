#ifndef __PLAYER_SEQUENCE_H
#define __PLAYER_SEQUENCE_H

#include "collection/parray.h"
#include "base/stream.h"

NS_FW_BASE_USE
namespace PLAYER {
	class Device;

	class Sequence : public Stream {
	//private: PROP_R(Device*, device);
	public:
		Sequence();
		Sequence(byte* stream, int offset = 0, int length = 0);
		virtual ~Sequence();

		void init(int length);

		Sequence* writeHeader();
		Sequence* writeDelta(word delta);
		Sequence* writeCommand(byte cmd);
		Sequence* writeEOF();
		Sequence* writeEOS();
		Sequence* writeByte(byte value);
		Sequence* writeString(char* str);
		Sequence* writeBytes(byte* stream, int length);
		Sequence* writeStream(Stream* stream, long length = 0, long offset = 0);
		Sequence* writeWord(word value);
		Sequence* writeDword(dword value);
		Sequence* writeFloat(float value);

		//char* print(Device* device = NULL);
	};
}

#endif