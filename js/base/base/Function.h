#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "base/Object.h"

NS_FW_BASE_BEGIN

//class Array;
//class Function_ : public Object_ {
//	friend class Function;
//protected:
//	Array* arguments_;
//	Function_();
//	virtual ~Function_();
//};

typedef Object* Function(size_t, ...);

//class Function : public Object {
//public:
//	Function();
//	virtual ~Function();
//
//	Object operator()(size_t, ...);
//};

NS_FW_BASE_END

#endif