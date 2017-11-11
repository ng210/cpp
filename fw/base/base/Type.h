#ifndef __TYPE_H
#define __TYPE_H

#include "base/base_defs.h"

NS_FW_BASE_BEGIN

#define MAX_TYPE_COUNT 256

class RunTime;
/*****************************************************************************
* Type
*****************************************************************************/
class TypeMgr;
class Type {
	friend class RunTime;
	static size_t typeCount_;
	static Type types_[];
	static void initialize();
	static void shutDown();

	size_t code_;
	const char* name_;
	const Type* parentType_;
protected:
	Type(void);
public:
	static Type* add(const char*, const char* = NULL);
	static Type* getTypeByName(const char*);

	const size_t code() const;
	const char* name() const;
	const Type* parentType() const;
	bool instanceOf(const char*) const;
	bool instanceOf(size_t) const;
};

NS_FW_BASE_END

#endif