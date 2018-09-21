#ifndef __MAP_H
#define __MAP_H

#include "collection/array.h"
#include "collection/keyvaluepair.h"

NS_FW_BASE_BEGIN

typedef size_t(HashingFunction)(void* key, size_t itemSize);

#define MAP_USE_REF -1

//*****************************************************************************
// Map
//*****************************************************************************
class Map {
	friend class RunTime;
private: PROP_R(ArrayBase*, keys);
private: PROP_R(ArrayBase*, values);
private: PROP_R(size_t, size);
	//static const size_t seedExt_;
	//static const size_t seedInt_;
	static int compareWrapper_(void* item, UINT32 ix, Collection* collection, void* key);
protected:
	HashingFunction* hashing_;
	Array* bucketList_;
	//static void initialize();
	//static void shutDown();
	Map() {};
	void init(UINT32 keySize, UINT32 valueSize, HashingFunction hashing, CollectionCallback* compare);
public:
	Map(UINT32 keySize, UINT32 valueSize = sizeof(void*), HashingFunction hashing = Map::hashingStr, CollectionCallback* compare = Collection::compareByRef);
	//Map(Array*, const char* = "=");
	//Map(Array*, const wchar_t* = L"=");
	virtual ~Map();
	void compare(CollectionCallback callback);
	void hashing(HashingFunction callback);

	bool containsKey(void* key);
	//void cleanUp();
	void* get(void* key);
	void* put(KeyValuePair* keyValue);
	void* put(void* key, void* value);

	static HashingFunction hashingItem;
	static HashingFunction hashingStr;
};

#define MAP_FOREACH(map, ACTION) for (UINT32 i = 0; i < (map)->keys()->length(); i++) { \
	void* key = (map)->keys()->getAt(i);	\
	void* value = (map)->get(key); \
	ACTION; \
}


NS_FW_BASE_END

#endif