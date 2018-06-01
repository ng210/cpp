#include "collection/map.h"
#include "collection/parray.h"
#include "base/memory.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

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
	return bucket->compare()(keyValue->key(), ix, bucket, key);
}

Map::Map(UINT32 keySize, UINT32 valueSize, HashingFunction hashing, CollectionCallback* compare) {
	init(keySize, valueSize, hashing, compare);
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

void Map::init(UINT32 keySize, UINT32 valueSize, HashingFunction hashing, CollectionCallback* compare) {
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
		bucketList_->add(bucket);
		ptr++;
	}
	hashing_ = hashing;
}

Map::~Map() {
	DEL_(keys_);
	DEL_(values_);
	ARRAY_FOREACH(bucketList_, ((Array*)value)->~Array());
	DEL_(bucketList_);
}
//
//void Map::cleanUp() {
//	for (size_t i = 0; i < MAP_BUCKETLIST_SIZE; i++) {
//		((Array*)bucketList_->get(i))->cleanUp();
//	}
//}

void Map::compare(CollectionCallback callback) {
	keys_->compare(callback);
	ARRAY_FOREACH(bucketList_, ((Array*)value)->compare(callback));
}
void Map::hashing(HashingFunction callback) {
	hashing_ = callback;
}
void* Map::get(void* key) {
	size_t hash = hashing_(key, keys_->itemSize());
	UINT32 ix = (UINT32)(hash % bucketList_->length());
	Array* bucket = (Array*)bucketList_->getAt(ix);
	int pos = -1;
	return bucket->binSearch(key, pos, Map::compareWrapper_) ? ((KeyValuePair*)bucket->getAt(pos))->value_ : NULL;
}
void* Map::put(KeyValuePair* keyValue) {
	return put(keyValue->key(), keyValue->value());
}
void* Map::put(void* key, void* value) {
	size_t hash = hashing_(key, keys_->itemSize());
	UINT32 ix = (UINT32)(hash % bucketList_->length());
	Array* bucket = (Array*)bucketList_->getAt(ix);
	int pos = 0;
	void* res = NULL;
	KeyValuePair* keyValue = (KeyValuePair*)bucket->binSearch(key, pos, Map::compareWrapper_);
	if (keyValue == NULL) {
		ix = values_->length();
		KeyValuePair kvp(keys_->add(key), values_->add(value));
		kvp.index_ = ix;
		bucket->insertAt(pos, &kvp);
		size_++;
	} else {
		// key already exists
		res = keyValue->value_;
		keyValue->value_ = value;
		values_->setAt(keyValue->index(), value);
	}
	return res;
}


//Object* Map::operator[](Object* key) {
//	return get(key);
//}

NS_FW_BASE_END