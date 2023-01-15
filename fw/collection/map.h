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
class Map : public Collection {
	friend class RunTime;
private: PROP_R(ArrayBase*, keys);
private: PROP_R(ArrayBase*, values);
private: PROP_R(size_t, size);
	static int compareWrapper_(void* item, UINT32 ix, Collection* collection, void* key);
protected:
	Array* bucketList_;
	Array* getBucket(Key key);
public:
	Map() : keys_(NULL), values_(NULL), size_(0), bucketList_(NULL), hashing_(NULL) { };
	Map(UINT32 keySize, UINT32 valueSize = sizeof(void*), HashingFunction hashing = Map::hashingStr, COLLECTIONCALLBACK* compare = Collection::compareByRef);

	virtual ~Map();
	void initialize(UINT32 keySize, UINT32 valueSize, HashingFunction* hashing, COLLECTIONCALLBACK* compare);

	PROP(HashingFunction*, hashing);

	bool containsKey(void* key);
	//void cleanUp();
	void* put(KeyValuePair* keyValue);
	void* put(Key key, void* value);

	// Collection
	void* add(Key key, void* value = NULL);
	void* get(Key key);
	void* insert(Key key, void* item = NULL);
	void remove(Key key);
	void set(Key key, void* item);
	void set(void** oldItem, void* newItem);

	int apply(COLLECTIONCALLBACK, ...);
	void fill(void* value);
	void* search(Key key, int& ix, COLLECTIONCALLBACK* compare = NULL);

	static HashingFunction hashingItem;
	static HashingFunction hashingStr;
};

#define MAP_FOREACH(map, ACTION) for (int i = 0; i < (map)->keys()->length(); i++) { \
	void* key = (map)->keys()->get(i);	\
	void* value = (map)->get(key); \
	ACTION \
}


NS_FW_BASE_END

#endif