#ifndef __NULL_H
#define __NULL_H

#include "base/Object.h"

NAMESPACE_FRMWRK_BEGIN
/*****************************************************************************
* Object_ (structure)
*****************************************************************************/
class Null_ : public Object_ {
	friend class Null;
	static const Null_ instance_;
	Null_(void);
	~Null_(void);
	String toString();
};
/*****************************************************************************
* Null (reference)
*****************************************************************************/
class Null : public Object {
protected:
	Null(void);
	~Null(void);
public:
	static const Null instance_;

	//operator Object();

};
#define null Null::instance_

NAMESPACE_FRMWRK_END

#endif