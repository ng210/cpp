#ifndef __PLAYER_INPUT_EXT_H
#define __PLAYER_INPUT_EXT_H

#include "basedef.h"
#include "base/handler.h"
#include "player/src/input.h"

NS_FW_BASE_USE

namespace PLAYER {

	class InputExt {
	public:
        static void set(Input* input, Value min, Value max, Value step, Value value);
        static void setFromNormalized(Input* input, float v);
        static void getValueAsString(Input* input, char* str, int len);
        static float getNormalized(Input* input);
        static int writeToStream(Input* input, byte*& stream);
        static int writeValueToStream(Input* input, byte*& stream);
        static bool compare(InputBase* input, int fieldId, Value value);
	};
}

#endif