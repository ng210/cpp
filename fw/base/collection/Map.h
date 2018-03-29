#ifndef __MAP_H
#define __MAP_H

#include "collection/array.h"
#include "collection/keyvaluepair.h"

NS_FW_BASE_BEGIN
//*****************************************************************************
// Map
//*****************************************************************************
class Map {
	friend class RunTime;
private: PROP_R(Array*, keys);
private: PROP_R(Array*, values);
private: PROP_R(size_t, size);
	static const size_t seedExt_;
	static const size_t seedInt_;
	static int compareWrapper_(void* item, unsigned int ix, Collection* map, void* key);
protected:
	Array* bucketList_;
	//static void initialize();
	//static void shutDown();
	static size_t hashing_(void*, size_t, size_t);
	void init(unsigned int keySize, unsigned int valueSize, CollectionCallback* compare);
public:
	Map(unsigned int keySize = sizeof(void*), unsigned int valueSize = sizeof(void*), CollectionCallback* compare = Map::compareByRef);
	//Map(Array*, const char* = "=");
	//Map(Array*, const wchar_t* = L"=");
	virtual ~Map();
	void compare(CollectionCallback callback);

	//void cleanUp();
	void* get(void* key);
	void* put(KeyValuePair* keyValue);
	void* put(void* key, void* value);

	static CollectionCallback compareStr;
	static CollectionCallback compareByRef;
};

#define MAP_FOREACH(map, ACTION) for (UINT32 i = 0; i < map->keys()->length(); i++) { \
	void* key = map->keys()->getAt(i);	\
	void* value = map->get(key); \
	ACTION; \
}


NS_FW_BASE_END

#endif