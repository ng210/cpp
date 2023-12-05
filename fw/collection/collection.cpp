# include <stdarg.h>
#include "collection/collection.h"

NS_FW_BASE_USE

Collection::Collection() {
	length_ = 0;
	compare_ = compareByRef;
}

Collection::~Collection() {
}

void Collection::fill(void* fillValue) {
	apply(
		[](COLLECTION_ARGUMENTS) {
			var value_ = va_arg(args, void*);
			collection->set(value, value_);
			return (void*)1;
		},
		fillValue
	);
}

void Collection::from(Collection* collection) {
	collection->apply(
		[](COLLECTION_ARGUMENTS) {
			var that = va_arg(args, Collection*);
			return that->add(key, value);
		},
		this
	);
}

void* Collection::search(Key key, Key& found, COLLECTION_COMPARE* compare) {
	void* value = NULL;
	apply(
		[](void* item, Key key, Collection* collection, va_list args) {
			var key_ = va_arg(args, Key);
			var compare_ = va_arg(args, COLLECTION_COMPARE*);
			var pValue_ = va_arg(args, void**);
			var res = compare_(item, key, collection, NULL);
			if (res == 0) {
				*pValue_ = item;
			}
			return *pValue_;
		}, key, compare, &value);
	return value;
}

int Collection::compareByRef(COLLECTION_ARGUMENTS) {
	return (int)((size_t)value - (size_t)key.p);
}
int Collection::compareStr(COLLECTION_ARGUMENTS) {
	return strncmp((char*)value, (char*)key.p);
}
int Collection::compareStrReverse(COLLECTION_ARGUMENTS) {
	return strncmp((char*)key.p, (char*)value);
}
int Collection::compareInt(COLLECTION_ARGUMENTS) {
	int i = *(int*)value;
	return i - key.i;
}
