#include <stdarg.h>
#include "collection/map.h"
#include "collection/parray.h"
#include "base/memory.h"
#include "base/str.h"

NS_FW_BASE_USE

#define MAP_BUCKETLIST_SIZE 16
#define MAP_BUCKET_SIZE 64
#define MAP_ITEM_COUNT (MAP_BUCKETLIST_SIZE * MAP_BUCKET_SIZE)

#define MAP_SEED1 127513
#define MAP_SEED2 0x3cb7a7f6d295ef7ULL

//const size_t Map::seedExt_ = 0x80000000000003ULL;
//const size_t Map::seedInt_ = 2;

//void Map::initialize() {
//	// add type entry
//	classType_ = ADD_TYPE(Map);
//	Map::defaultSeparator_ = NEW_(String, "=");
//}
//void Map::shutDown() {
//	DEL_(Map<K,V>::defaultSeparator_);
//}

size_t Map::hashingItem(void* key, size_t itemSize) {
	size_t hash = 0;
	UINT8* b = (UINT8*)key;
	for (int i = 0; i < itemSize; i++) {
		hash = ((hash + *b++) * MAP_SEED1) ^ MAP_SEED2;
	}
	return hash;
}
size_t Map::hashingStr(void* key, size_t unused1) {
	size_t hash = 0;
	UINT32 len = 0;
	char* str = (char*)key;
	char ch;
	while ((ch = str[len]) != '\0') {
		hash = ((hash + ch) * MAP_SEED1) ^ MAP_SEED2;
		len++;
	}
	hash = ((hash + len) * MAP_SEED1) ^ MAP_SEED2;
	return hash;
}

int Map::compareWrapper_(void* item, UINT32 ix, Collection* collection, void* key) {
	Array* bucket = (Array*)collection;
	KeyValuePair* keyValue = (KeyValuePair*)item;
	return bucket->compare()(keyValue->key().p, ix, bucket, key);
}

Map::Map(UINT32 keySize, UINT32 valueSize, HashingFunction* hashing, COLLECTIONCALLBACK* compare) {
	initialize(keySize, valueSize, hashing, compare);
}
//Map::Map(Array* array, const char* separator) {
//	init();
//	for (size_t i = 0; i < array->length(); i++) {
//		Array* entry = ((String*)(*array)[i])->split(separator);
//		if (entry->length() > 0) {
//			if (entry->length() == 1) {
//				entry->add(NEW_(String, ""));
//			}
//			put(((String*)entry->get(0))->trim(), ((String*)entry->get(1))->trim());
//		}
//		entry->cleanUp();
//		DEL_(entry);
//	}
//}

void Map::initialize(UINT32 keySize, UINT32 valueSize, HashingFunction* hashing, COLLECTIONCALLBACK* compare) {
	if (keySize == MAP_USE_REF) keys_ = NEW_(PArray, MAP_ITEM_COUNT);
	else keys_= NEW_(Array, keySize, MAP_ITEM_COUNT);
	keys_->compare(compare);
	if (valueSize == MAP_USE_REF) values_ = NEW_(PArray, MAP_ITEM_COUNT);
	else values_ = NEW_(Array, valueSize, MAP_ITEM_COUNT);
	size_ = 0;

	bucketList_ = NEW_(Array, sizeof(Array), MAP_BUCKETLIST_SIZE);
	Array* ptr = (Array*)bucketList_->data();
	for (size_t i = 0; i < MAP_BUCKETLIST_SIZE; i++) {
		Array* bucket = new (ptr) Array(sizeof(KeyValuePair), MAP_BUCKET_SIZE);
		bucket->compare(compare);
		bucketList_->push(bucket);
		ptr++;
	}
	hashing_ = hashing;
}

Map::~Map() {
	DEL_(keys_);
	DEL_(values_);
	bucketList_->apply(
		[](void* item, UINT32 ix, Collection* collection, void* args) {
			((Array*)item)->~Array();
			return 1;
		});
	DEL_(bucketList_);
}

bool Map::containsKey(void* key) {
	int ix;
	return keys_->binSearch(key, ix, keys_->compare());	//Collection::compareInt
}

Array* Map::getBucket(Key key) {
	size_t hash = hashing_(key.p, keys_->itemSize());
	UINT32 ix = (UINT32)(hash % bucketList_->length());
	return (Array*)bucketList_->get(ix);
}

void* Map::put(KeyValuePair* keyValue) {
	return add(keyValue->key(), keyValue->value());
}
void* Map::put(Key key, void* value) {
	return add(key, value);
}

#pragma region Collection
void* Map::add(Key key, void* value) {
	int pos = -1;
	var bucket = getBucket(key);
	var keyValue = (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
	void* res = NULL;
	if (keyValue == NULL) {
		var ix = values_->length();
		KeyValuePair kvp(keys_->push(key.p), values_->push(value));
		kvp.index_ = ix;
		bucket->insert(pos, &kvp);
		size_++;
		res = kvp.value_;
	}
	else {
		// key already exists
		res = keyValue->value_;
		keyValue->value_ = value;
		values_->set(keyValue->index(), value);
	}
	return res;
}
void* Map::insert(Key key, void* value) {
	// insertion is not supported
	return add(key, value);
}
void Map::remove(Key key) {
	// unsorted Map, this method brakes the order of keys and values
	int pos = -1;
	var bucket = getBucket(key);
	var kvp = (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
	if (kvp) {
		// remove selected KeyValuePair
		bucket->remove(key);
		// reuse the freed index
		var ix = kvp->index_;
		// re-insert the last key - value
		var key = keys_->pop();
		keys_->set(ix, key);
		var value = values_->pop();
		values_->set(ix, value);
		// get KeyValuePair with the last index
		bucket = getBucket(key);
		kvp = (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
		// set index to the new value
		kvp->index_ = ix;
	}
}
void* Map::get(Key key) {
	int pos = -1;
	var bucket = getBucket(key);
	return bucket->binSearch(key, pos, Map::compareWrapper_) ? ((KeyValuePair*)bucket->get(pos))->value_ : NULL;
}
void Map::set(Key key, void* item) {
	int pos = -1;
	var bucket = getBucket(key);
	var kvp = (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
	if (kvp) {
		values_->set(kvp->index_, item);
	}
}
void Map::set(void** item, void* data) {
	memcpy(item, data, values_->itemSize());
}

int Map::apply(COLLECTIONCALLBACK callback, ...) {
	va_list args;
	va_start(args, callback);
	dword ix = 0;
	var res = -1;
	for (var i = 0; i < bucketList_->length(); i++) {
		var bucket = (Array*)bucketList_->get(i);
		for (var j = 0; j < bucket->length(); j++) {
			var kvp = (KeyValuePair*)bucket->get(j);
			if (!callback(kvp, ix, this, args)) {
				res = ix;
				break;
			}
			ix++;
		}
	}
	va_end(args);
	return res;
}
void Map::fill(void* value) {
	for (var i = 0; i < values_->length(); i++) {
		values_->set(i, value);
	}
}

void* Map::search(Key key, int& ix, COLLECTIONCALLBACK* compare) {
	void* res = NULL;
	int pos = -1;
	ix = -1;
	var bucket = getBucket(key);
	var kvp = (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
	if (kvp) {
		ix = pos;
		res = kvp->value();
	}
	return res;
}
#pragma endregion