#include "base/debug.h"
#include "base/memory.h"

#ifdef _DEBUG

NS_FW_BASE_BEGIN

Tracer* Debug::tracer_ = NULL;

void Debug::initialize(int options) {
	if (options & DEBUG_WINDOWS) {
		if (options & DEBUG_UNICODE) {
			Debug::tracer_ = new TracerWinW();
		} else {
			Debug::tracer_ = new TracerWinA();
		}
	} else {
		if (options & DEBUG_UNICODE) {
			Debug::tracer_ = new TracerW();
		} else {
			Debug::tracer_ = new TracerA();
		}
	}
	if (options & DEBUG_MEMORY) {
		MemoryMgr::isDebugOn = true;
	}
}

void Debug::destroy() {
	delete Debug::tracer_;
}

NS_FW_BASE_END

#endif