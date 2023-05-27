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


Map::Map() : keys_(NULL), values_(NULL), size_(0), bucketList_(NULL), hashing_(NULL) {
	hasRefKey_ = false;
}

//void Map::initialize() {
//	// add type entry
//	classType_ = ADD_TYPE(Map);
//	Map::defaultSeparator_ = NEW_(String, "=");
//}
//void Map::shutDown() {
//	DEL_(Map<K,V>::defaultSeparator_);
//}
size_t Map::hashingInt(Key key, size_t unused) {
	return (key.i * MAP_SEED1) ^ MAP_SEED2;
}
size_t Map::hashingItem(Key key, size_t itemSize) {
	size_t hash = 0;
	UINT8* b = (UINT8*)key.p;
	for (int i = 0; i < itemSize; i++) {
		hash = ((hash + *b++) * MAP_SEED1) ^ MAP_SEED2;
	}
	return hash;
}
size_t Map::hashingStr(Key key, size_t unused1) {
	size_t hash = 0;
	UINT32 len = 0;
	char* str = (char*)key.p;
	char ch;
	while ((ch = str[len]) != '\0') {
		hash = ((hash + ch) * MAP_SEED1) ^ MAP_SEED2;
		len++;
	}
	hash = ((hash + len) * MAP_SEED1) ^ MAP_SEED2;
	return hash;
}

int Map::compareWrapper_(COLLECTION_ARGUMENTS) {
	Array* bucket = (Array*)collection;
	KeyValuePair* keyValue = (KeyValuePair*)value;
	return bucket->compare()(keyValue->key(), key, bucket, args);
}

Map::Map(UINT32 keySize, UINT32 valueSize, HashingFunction* hashing, COLLECTION_COMPARE* compare) {
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

void Map::initialize(UINT32 keySize, UINT32 valueSize, HashingFunction* hashing, COLLECTION_COMPARE* compare) {
	if (keySize == MAP_USE_REF) keys_ = NEW_(PArray, MAP_ITEM_COUNT);
	else keys_= NEW_(Array, keySize, MAP_ITEM_COUNT);
	compare_ = compare;
	keys_->compare(compare);
	if (valueSize == MAP_USE_REF) values_ = NEW_(PArray, MAP_ITEM_COUNT);
	else values_ = NEW_(Array, valueSize, MAP_ITEM_COUNT);
	size_ = 0;
	hasRefKey_ = true;
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
		[](COLLECTION_ARGUMENTS) {
			((Array*)value)->~Array();
			return value;
		});
	DEL_(bucketList_);
}

bool Map::containsKey(Key key) {
	Key found = -1;
	return keys_->binSearch(key, found, keys_->compare());	//Collection::compareInt
}

Array* Map::getBucket(Key key) {
	size_t hash = hashing_(key, keys_->itemSize());
	UINT32 ix = (UINT32)(hash % bucketList_->length());
	return (Array*)bucketList_->get(ix);
}

KeyValuePair* Map::getKeyValuePair(Key key) {
	Key pos = -1;
	var bucket = getBucket(key);
	return (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
}

void* Map::put(KeyValuePair* keyValue) {
	return add(keyValue->key(), keyValue->value());
}
void* Map::put(Key key, void* value) {
	return add(key, value);
}

void Map::sort(COLLECTION_COMPARE* compare) {
	// sort keys, adjust KeyValuePairs in buckets
	for (var i = 0; i < values_->length(); i++) {
	}
}

#pragma region Collection
void* Map::add(Key key, void* value) {
	Key pos = -1;
	var bucket = getBucket(key);
	var keyValue = (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
	void* res = NULL;
	if (keyValue == NULL) {
		var ix = values_->length();
		KeyValuePair kvp(NULL, values_->push(value));
		if (hasRefKey_) {
			kvp.key_ = keys_->push(key.p);
		}
		else {
			kvp.key_ = keys_->push(&key.i);
		}

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
	Key removeIx = -1;
	var bucket = getBucket(key);
	if (bucket) {
		var kvp = (KeyValuePair*)bucket->binSearch(key, removeIx, Map::compareWrapper_);
		if (kvp) {
			void *pKey = kvp->key(), *pValue = kvp->value();
			int ix = kvp->index();
			// remove selected KeyValuePair
			bucket->remove(removeIx);
			size_--;
			// move the last key and value
			if (size_ > 0) {
				// if map still has items and
				var pLastKey = keys_->pop();
				var pLastValue = values_->pop();
				// the removed item was not the last one
				if (ix < size_) {
					// not the last item was removed
					if (pLastKey != pKey) {
						Key lastKey = NULL;
						if (hasRefKey_) {
							keys_->set(ix, pLastKey);
							lastKey.p = pLastKey;
						}
						else {
							keys_->set(ix, pLastKey);
							memcpy(&lastKey.bytes, pLastKey, keys_->itemSize());
						};
						values_->set(ix, pLastValue);

						// update the members of linked KeyValuePair
						bucket = getBucket(lastKey);
						Key pos = -1;
						kvp = (KeyValuePair*)bucket->binSearch(lastKey, pos, Map::compareWrapper_);
						kvp->key_ = keys_->get(ix);
						kvp->value_ = values_->get(ix);
						kvp->index_ = ix;
					}
				}
			}
		}
	}
}
void* Map::get(Key key) {
	var kvp = getKeyValuePair(key);
	return kvp != NULL ? kvp->value_ : NULL;
}
void Map::set(Key key, void* item) {
	Key pos = -1;
	var bucket = getBucket(key);
	var kvp = (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
	if (kvp) {
		values_->set(kvp->index_, item);
	}
}
void Map::set(void** item, void* data) {
	memcpy(item, data, values_->itemSize());
}

int Map::apply(COLLECTION_ACTION action, ...) {
	va_list args;
	va_start(args, action);
	dword ix = 0;
	var res = -1;
	for (var i = 0; i < bucketList_->length(); i++) {
		var bucket = (Array*)bucketList_->get(i);
		for (var j = 0; j < bucket->length(); j++) {
			var kvp = (KeyValuePair*)bucket->get(j);
			if (!action(kvp, j, this, args)) {
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

void* Map::search(Key key, Key& found, COLLECTION_COMPARE* compare) {
	void* res = NULL;
	Key pos = -1;
	found = -1;
	var bucket = getBucket(key);
	var kvp = (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
	if (kvp) {
		found = pos;
		res = kvp->value();
	}
	return res;
}
#pragma endregion