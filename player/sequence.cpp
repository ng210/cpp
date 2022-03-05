#include <stdarg.h>
#include "sequence.h"
#include "base/memory.h"
//#include "utils/buffer.h"
//#include "base/debug.h"

NS_PLAYER_BEGIN

Sequence::Sequence() : frames_(sizeof(Frame)) {
}

Sequence::~Sequence() {
	//ARRAY_FOREACH(&frames_, ((Frame*)value)->destroy(););
	ARRAY_FOREACH(&frames_, DEL_((Frame*)value););
}

Frame* Sequence::frame(int ix) {
	return (Frame*)frames_.getAt(ix);
}

#ifdef _EDITOR	// Editor extensions
Frame* Sequence::addFrame(int time) {
	int pos = frames_.length();
	int elapsed = 0;
	ARRAY_FOREACH(&frames_, int delta = ((Frame*)value)->delta_; if (elapsed + delta > time) { pos = i; break; } elapsed += delta; );
	Frame frame;
	frame.delta_ = time - elapsed;
	return (Frame*)frames_.insertAt(pos, &frame);
}

Sequence::fromStream(UINT8* bytes, IAdapter* adapter) {
	// create from binary sequence
	PLAYER_FRAME_BIN_U stream;
	stream.bytes = bytes;
	PLAYER_COMMAND_U cmd;
	Frame* frame;
	do {
		frame = NEW_(Frame);
		// write first delta into frame.delta
		frame->delta_ = stream.frame->delta;
		do {

		} while (stream.frame->delta == 0);

	} while (cmd.s.id != Player_Cmd_end);
}

int Sequence::toStream(IAdapter* adapter, UINT8* stream) {
	int totalLength = 0;
	for (UINT32 i = 0; i < frames_.length(); i++) {
		Frame* frame = (Frame*)frames_.getAt(i);
		UINT8 tmp[4];
		*(UINT16*)&tmp[0] = frame->delta_;
		tmp[2] = frame->commands()->length();
		buffer->append(tmp, sizeof(UINT8) + sizeof(UINT16));
		totalLength += sizeof(UINT8) + sizeof(UINT16);
		for (UINT32 c = 0; c < frame->commands()->length(); c++) {
			PLAYER_COMMAND cmd = (PLAYER_COMMAND)frame->commands()->getAt(c);
			totalLength += adapter->dumpCommand(cmd, buffer);
		}
	}
	LOG("%d UINT8s written\n", totalLength);
	return totalLength;
}

#ifdef _DEBUG
void Sequence::print(IAdapter* adapter) {
	int ts = 0;
	for (UINT32 i = 0; i < frames_.length(); i++) {
		Frame* frame = (Frame*)frames_.getAt(i);
		ts += frame->delta_;
		LOG(".Frame #%04d: ", ts);
		for (UINT32 c = 0; c < frame->commands()->length(); c++) {
			PLAYER_COMMAND cmd = (PLAYER_COMMAND)frame->commands()->getAt(c);
			char* str = adapter->logCommand(cmd);
			LOG(" %s", str);
			FREE(str);
		}
		LOG("\n");
	}
	LOG("\n");
}
#endif

#endif

NS_PLAYER_END