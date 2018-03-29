#ifndef __BOOLEAN_H
#define __BOOLEAN_H

#include "base/Object.h"

NS_FW_BASE_BEGIN

#define TYPE_BOOLEAN GET_TYPE(Boolean)

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

	bool toBool();
	void* valueOf();
};

#define True Boolean::trueInstance_
#define False Boolean::falseInstance_

NS_FW_BASE_END

#endif