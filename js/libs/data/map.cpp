#include "collection/Map.h"
#include "base/String.h"
#include "collection/Array.h"
#include "base/MemoryMgr.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

#define MAP_BUCKETLIST_SIZE 4
#define MAP_BUCKET_SIZE 4

const Type* KeyValuePair::classType_;

KeyValuePair::KeyValuePair(Object* key, Object* value) {
	key_ = key;
	value_ = value;
}

const Type* Map::classType_;
String* Map::defaultSeparator_ = NULL;

const size_t Map::seedExt_ = 0x80000000000003ULL;
//const size_t Map::seedInt_ = 2;

void Map::initialize() {
	// add type entry
	classType_ = ADD_TYPE(Map);
	Map::defaultSeparator_ = NEW_(String, "=");
}
void Map::shutDown() {
	DEL_(Map::defaultSeparator_);
}

size_t Map::hashing_(Object* key, size_t length, size_t seed) {
	return (key->hash()*seed) & (length - 1);
}

int Map::compareByValue(Object* a, Object* b) {
	char* str = a->toString();
	int res = ((KeyValuePair*)b)->key()->strcmp(str);
	FREE(str);
	return res;
}

Map::Map() {
	init();
}

Map::Map(Array* array, String* separator) {
	init();
	for (size_t i = 0; i < array->length(); i++) {
		Array* entry = ((String*)(*array)[i])->split(separator);
		if (entry->length() > 0) {
			if (entry->length() == 1) {
				entry->push(NEW_(String, ""));
			}
			put(((String*)entry->get(0))->trim(), ((String*)entry->get(1))->trim());
		}
		entry->cleanUp();
		DEL_(entry);
	}
}

void Map::init() {
	compare_ = Map::compareByValue;
	keys_ = NEW_(Array);
	values_ = NEW_(Array);
	size_ = 0;
	bucketList_ = NEW_(Array, (size_t)MAP_BUCKETLIST_SIZE);
	for (size_t i = 0; i < MAP_BUCKETLIST_SIZE; i++) {
		bucketList_->push(NEW_(Array, (size_t)MAP_BUCKET_SIZE));
	}
}

Map::~Map() {
	DEL_(keys_);
	DEL_(values_);
	bucketList_->cleanUp();
	DEL_(bucketList_);
}

void Map::cleanUp() {
	for (size_t i = 0; i < MAP_BUCKETLIST_SIZE; i++) {
		((Array*)bucketList_->get(i))->cleanUp();
	}
}
Object* Map::get(Object* key) {
	size_t ix = Map::hashing_(key, bucketList_->length(), Map::seedExt_);
	Array* bucket = (Array*)bucketList_->get(ix);
	long long pos = bucket->search(key, Map::compareByValue);
	return pos >= 0 ? ((KeyValuePair*)bucket->get(pos))->value() : Null;
}

Object* Map::put(Object* key, Object* value) {
	size_t ix = Map::hashing_(key, bucketList_->length(), Map::seedExt_);
	Array* bucket = (Array*)bucketList_->get(ix);
	long long pos = bucket->search(key, compare_);
	Object* res = NULL;
	if (pos < 0) {
		size_t ix = (size_t)(-pos - 1);
		KeyValuePair* keyValue = NEW_(KeyValuePair, key, value);
		Array* arr = bucket->splice(ix, 0, 1, keyValue);
		DEL_(arr);
		keyValue->index_ = keys_->length();
		keys_->push(key);
		values_->push(value);
		size_++;
	} else {
		// key already exists
		KeyValuePair* keyValue = (KeyValuePair*)bucket->get(pos);
		res = keyValue->value_;
		keyValue->value_ = value;
		values_->put(keyValue->index_, value);
	}
	return res;
}
Object* Map::operator[](Object* key) {
	return get(key);
}

NS_FW_BASE_END