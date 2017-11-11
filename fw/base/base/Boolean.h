#ifndef __BOOLEAN_H
#define __BOOLEAN_H

#include "base/Object.h"

#define TYPE_BOOLEAN ((Type*)Boolean::classType())->code()

NS_FW_BASE_BEGIN

class RunTime;
class Boolean : public Object {
	friend class RunTime;
	DECL_TYPE(Boolean);
	static void initialize();
	static void shutDown();
	static Boolean* trueInstance_;
	static Boolean* falseInstance_;
	bool value_;
public:
	Boolean();
	Boolean(bool);
	virtual ~Boolean();
};

#define True Boolean::trueInstance_
#define False Boolean::falseInstance_

NS_FW_BASE_END

#endif