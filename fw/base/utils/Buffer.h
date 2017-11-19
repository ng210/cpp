#ifndef __BUFFER_H
#define __BUFFER_H

#include "base/base_defs.h"
#include "base/Object.h"

NS_FW_BASE_BEGIN

#define TYPE_BUFFER GET_TYPE(Buffer)

class Buffer : public Object {
	friend class RunTime;
	DECL_TYPE(Buffer);
	PROP_R(char*, buffer);
	PROP_R(size_t, length);
	PROP_R(const Type*, type);
	PROP_R(size_t, position);

	void init(const Type*, const size_t);
public:
	Buffer();
	Buffer(const Type*, size_t);
	virtual ~Buffer();

	size_t getSize();

	void set(char, size_t);
	void set(int, size_t);
	void set(size_t, size_t);
	void set(float, size_t);
	void set(double, size_t);
	void set(Object*, size_t);

	void push(char);
	void push(int);
	void push(size_t);
	void push(float);
	void push(double);
	void push(Object*);
};

NS_FW_BASE_END

#endif