#ifndef __PLAYER_FRAME_H
#define __PLAYER_FRAME_H

#include "playerdef.h"

NS_FW_BASE_USE
NS_PLAYER_BEGIN

class Frame {
private: PROP_R(PArray*, commands);

public:
	int delta_;
	Frame();
	~Frame();
	void destroy();

	void addCommand(PLAYER_COMMAND cmd);
	void empty();
	static PLAYER_COMMAND makeCommand(int code, int argc, ...);
};

#define MAKE_COMMAND0(code) Frame::makeCommand(code, 0)
#define MAKE_COMMAND1(code, ...) Frame::makeCommand(code, 1, __VA_ARGS__)
#define MAKE_COMMAND2(code, ...) Frame::makeCommand(code, 2, __VA_ARGS__)
#define MAKE_COMMAND3(code, ...) Frame::makeCommand(code, 3, __VA_ARGS__)
#define MAKE_COMMAND4(code, ...) Frame::makeCommand(code, 4, __VA_ARGS__)

NS_PLAYER_END

#endif