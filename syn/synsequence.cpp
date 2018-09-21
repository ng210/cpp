#include <stdarg.h>
#include "synsequence.h"
#include "base/memory.h"
#include "base/string.h"
#include "base/debug.h"
#include <stdio.h>

NS_SYN_BEGIN

//Frame* SynSequence::addFrame(int time) {
//	int pos = frames_.length();
//	int elapsed = 0;
//	ARRAY_FOREACH(&frames_, int delta = ((Frame*)value)->delta_; if (elapsed + delta > time) { pos = i; break; } elapsed += delta; );
//	Frame frame;
//	frame.delta_ = time - elapsed;
//	return (Frame*)frames_.insertAt(pos, &frame);
//}
//
//Frame* SynSequence::frame(int ix) {
//	return (Frame*)frames_.getAt(ix);
//}

//int SynSequence::toStream(IAdapter* adapter, Buffer* buffer) {
//	int totalLength = 0;
//	for (UINT32 i = 0; i < frames_.length(); i++) {
//		Frame* frame = (Frame*)frames_.getAt(i);
//		UINT8 tmp[4];
//		*(UINT16*)&tmp[0] = frame->delta_;
//		tmp[2] = frame->commands()->length();
//		buffer->append(tmp, sizeof(UINT8) + sizeof(UINT16));
//		totalLength += sizeof(UINT8) + sizeof(UINT16);
//		for (UINT32 c = 0; c < frame->commands()->length(); c++) {
//			PLAYER_COMMAND cmd = (PLAYER_COMMAND)frame->commands()->getAt(c);
//			totalLength += adapter->dumpCommand(cmd, buffer);
//		}
//	}
//	LOG("%d UINT8s written\n", totalLength);
//	return totalLength;
//}

char* SynSequence::info(IAdapter* adapter) {
	int ts = 0;
	PArray stringBuilder;
	for (UINT32 i = 0; i < frames_.length(); i++) {
		Frame* frame = (Frame*)frames_.getAt(i);
		ts += frame->delta_;
		stringBuilder.add(str_format(".Frame #%04d: ", ts));
		for (UINT32 c = 0; c < frame->commands()->length(); c++) {
			PLAYER_COMMAND cmd = (PLAYER_COMMAND)frame->commands()->getAt(c);
			stringBuilder.add(adapter->logCommand(cmd));
		}
	}
	return stringBuilder.str_join("\n");
}

NS_PLAYER_END