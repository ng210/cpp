#ifndef __MAP_H
#define __MAP_H

#include "base/Object.h"
#include "collection/Array.h"

NS_FW_BASE_BEGIN

class Map;
class KeyValuePair : public Object {
	friend class RunTime;
	friend class Map;
	DECL_TYPE(KeyValuePair);
protected: PROP_R(Object*, key);
protected: PROP_R(Object*, value);
protected:
	size_t index_;
public:
	KeyValuePair(Object*, Object*);
};

class Array;
class Map : public Object {
	friend class RunTime;
	DECL_TYPE(Map);
private: PROP_R(Array*, keys);
private: PROP_R(Array*, values);
private: PROP_R(size_t, size);
private: PROP(Compare*, compare);
	static const size_t seedExt_;
	static const size_t seedInt_;
	static int compareByValue(Object*, Object*);
protected:
	Array* bucketList_;
	static String* defaultSeparator_;
	static void initialize();
	static void shutDown();
	static size_t hashing_(Object*, size_t, size_t);
	void init();
public:
	Map();
	Map(Array*, String* = Map::defaultSeparator_);
	virtual ~Map();

	Object* operator[](Object*);
	void cleanUp();
	Object* get(Object*);
	Object* put(Object*, Object*);
};

NS_FW_BASE_END

#endif