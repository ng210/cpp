#ifndef __TYPE_H
#define __TYPE_H

#include "base/base_defs.h"

NS_FW_BASE_BEGIN

#define MAX_TYPE_COUNT 256

#define GET_TYPE(t) (t::classType())
#define ADD_TYPE(t) Type::add(STRINGIFY(NS_FW_BASE)"::" STRINGIFY(t), sizeof(t));

#define TYPE_CHAR (Type::getChar())
#define TYPE_BOOL (Type::getBool())
#define TYPE_SHORT (Type::getShort())
#define TYPE_INT (Type::getInt())
#define TYPE_SIZE_T (Type::getSize_t())
#define TYPE_FLOAT (Type::getFloat())
#define TYPE_DOUBLE (Type::getDouble())

class RunTime;
class Object;
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

	static const Type* char_;
	static const Type* bool_;
	static const Type* short_;
	static const Type* int_;
	static const Type* size_t_;
	static const Type* float_;
	static const Type* double_;

	size_t code_;
	size_t size_;
	const char* name_;
	const Type* parentType_;
protected:
	Type(void);
public:
	static Type* add(const char*, size_t, const char* = NULL);
	static Type* getTypeByName(const char*);

	const size_t code() const;
	const size_t size() const;
	const char* name() const;
	const Type* parentType() const;

	static bool instanceOf(Object*, const char*);
	static bool instanceOf(Object*, size_t);

	static const Type* getChar();
	static const Type* getBool();
	static const Type* getShort();
	static const Type* getInt();
	static const Type* getSize_t();
	static const Type* getFloat();
	static const Type* getDouble();

	//static bool toBool(Object*);
};

NS_FW_BASE_END

#endif