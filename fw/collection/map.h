#ifndef __MAP_H
#define __MAP_H

#include "collection/array.h"
#include "collection/keyvaluepair.h"

NS_FW_BASE_BEGIN

typedef size_t(HashingFunction)(Key key, size_t itemSize);

#define MAP_USE_REF -1

//*****************************************************************************
// Map
//*****************************************************************************
class Map : public Collection {
	friend class RunTime;
	static int compareWrapper_(COLLECTION_ARGUMENTS);
protected: PROP_R(ArrayBase*, keys);
protected: PROP_R(ArrayBase*, values);
protected: PROP_R(size_t, size);
protected:
	Array* bucketList_;
	Array* getBucket(Key key);
	KeyValuePair* getKeyValuePair(Key key);
protected: PROP(bool, hasRefKey);
public:
	Map();
	Map(UINT32 keySize, UINT32 valueSize = sizeof(void*), HashingFunction hashing = Map::hashingStr, COLLECTION_COMPARE* compare = Collection::compareByRef);

	virtual ~Map();
	void initialize(UINT32 keySize, UINT32 valueSize, HashingFunction* hashing, COLLECTION_COMPARE* compare);

	PROP(HashingFunction*, hashing);

	bool containsKey(Key key);
	//void cleanUp();
	void* put(KeyValuePair* keyValue);
	void* put(Key key, void* value);
	void sort(COLLECTION_COMPARE* compare = NULL);

	// Collection
	void* add(Key key, void* value = NULL);
	void* get(Key key);
	void* insert(Key key, void* item = NULL);
	void remove(Key key);
	void set(Key key, void* item);
	void set(void** oldItem, void* newItem);

	int apply(COLLECTION_ACTION* action, ...);
	void fill(void* value);
	Collection* map(COLLECTION_ACTION* action, bool removeNull, ...);
	Collection* map(COLLECTION_ACTION* action, int valueSize);
	void* search(Key key, Key& found, COLLECTION_COMPARE* compare = NULL);

	static HashingFunction hashingInt;
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