#ifndef __OBJECT_H
#define __OBJECT_H

#include "base/base_defs.h"
#include "base/Ref.h"

NAMESPACE_FRMWRK_BEGIN

class Object_;
class Object : public Ref<Object_> {
public:
	Object(void);
	virtual ~Object(void);

	virtual char* toCString();
	virtual int compareTo(Object& ref);
	virtual bool operator==(Object& ref);
	virtual operator const char*();
};

class Object_ : public Ref_ {
	friend class Object;
	friend class Ref<Object_>;

	PROP_R(protected, long long, Hash, i);

protected:
	Object_(void);
	~Object_(void);
public:
	bool operator==(Object_ *obj);
	operator const char*();
	char* toCString();
	int compareTo(Object_ *obj);
};

NAMESPACE_FRMWRK_END

#endif