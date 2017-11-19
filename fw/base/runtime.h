#ifndef __RUNTIME_H
#define __RUNTIME_H

#include "base/MemoryMgr.h"
#include "base/Type.h"
#include "base/str.h"
#include "base/String.h"
#include "base/Boolean.h"
#include "base/Number.h"
#include "base/Integer.h"
#include "base/Float.h"
#include "collection/Array.h"
//#include "rt/Date.h"
//#include "rt/Boolean.h"
//#include "rt/RegEx.h"
#include "utils/PathInfo.h"
#include "utils/Buffer.h"
#include "utils/File.h"

NS_FW_BASE_BEGIN

class RunTime {
public:
	static void initialize();
	static void shutDown();
};

NS_FW_BASE_END

#endif