#include "collection/map.h"
#include "base/memory.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

#define MAP_BUCKETLIST_SIZE 4
#define MAP_BUCKET_SIZE 4

const size_t Map::seedExt_ = 0x80000000000003ULL;
//const size_t Map::seedInt_ = 2;

//void Map::initialize() {
//	// add type entry
//	classType_ = ADD_TYPE(Map);
//	Map::defaultSeparator_ = NEW_(String, "=");
//}
//void Map::shutDown() {
//	DEL_(Map<K,V>::defaultSeparator_);
//}

size_t Map::hashing_(void* key, size_t length, size_t seed) {
	return ((size_t)key*seed) & (length - 1);
}

int Map::compareByRef(void* item, UINT32 ix, Collection* map, void* key) {
	return (int)((size_t)item - (size_t)key);
}

int Map::compareStr(void* item, UINT32 ix, Collection* map, void* key) {
	return strncmp(*(char**)item, *(char**)key);
}


int Map::compareWrapper_(void* item, UINT32 ix, Collection* map, void* key) {
	return ((Array*)map)->compare()(((KeyValuePair*)item)->key(), ix, map, key);
}

//int Map::compareByValue(void* item, UINT32 ix, Collection* map, va_list args) {
//	void* match = va_arg(args, void*);
//	return str1->compareTo(match);
//}

Map::Map(UINT32 keySize, UINT32 valueSize, CollectionCallback* compare) {
	init(keySize, valueSize, compare);
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

void Map::init(UINT32 keySize, UINT32 valueSize, CollectionCallback* compare) {
	keys_ = NEW_(Array, keySize);
	keys_->compare(compare);
	values_ = NEW_(Array, valueSize);
	size_ = 0;
	bucketList_ = NEW_(Array, sizeof(Array), MAP_BUCKETLIST_SIZE);
	Array* ptr = (Array*)bucketList_->data();
	for (size_t i = 0; i < MAP_BUCKETLIST_SIZE; i++) {
		Array* bucket = new (ptr) Array(sizeof(KeyValuePair), MAP_BUCKET_SIZE);	// NEW_(Array, sizeof(KeyValuePair), MAP_BUCKET_SIZE);
		//Array* bucket = NEW_(Array, sizeof(KeyValuePair), MAP_BUCKET_SIZE);
		bucket->compare(compare);
		bucketList_->add(bucket);
		ptr++;
	}
}

Map::~Map() {
	DEL_(keys_);
	DEL_(values_);
	//bucketList_->cleanUp();
	for (UINT32 i = 0; i < bucketList_->length(); i++) {
		Array* bucket = (Array*)bucketList_->getAt(i);
		bucket->~Array();
	}
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
}
void* Map::get(void* key) {
	UINT32 ix = (UINT32)Map::hashing_(key, bucketList_->length(), Map::seedExt_);
	Array* bucket = (Array*)bucketList_->getAt(ix);
	int pos = -1;
	return bucket->binSearch(key, pos, Map::compareWrapper_) ? ((KeyValuePair*)bucket->getAt(pos))->value_ : NULL;
}
void* Map::put(KeyValuePair* keyValue) {
	void* key = keyValue->key();
	void* value = keyValue->value();
	UINT32 ix = (UINT32)Map::hashing_(key, bucketList_->length(), Map::seedExt_);
	Array* bucket = (Array*)bucketList_->getAt(ix);
	int pos = 0;
	void* res = NULL;
	if (!bucket->binSearch(key, pos, Map::compareWrapper_)) {
		UINT32 length = keys_->length();
		key = keys_->add(key);
		value = values_->add(value);
		KeyValuePair kvp(key, value);
		kvp.index_ = length;
		bucket->insertAt(pos, &kvp);
		size_++;
	}
	else {
		// key already exists
		KeyValuePair* keyValue = (KeyValuePair*)bucket->getAt(pos);
		res = keyValue->value_;
		keyValue->value_ = value;
		values_->setAt(keyValue->index(), value);
	}
	return res;
}
void* Map::put(void* key, void* value) {
	KeyValuePair keyValue(key, value);
	return put(&keyValue);
}


//Object* Map::operator[](Object* key) {
//	return get(key);
//}

NS_FW_BASE_END