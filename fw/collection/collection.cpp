#include "collection/collection.h"

NS_FW_BASE_USE

Collection::Collection() {
	length_ = 0;
	compare_ = compareByRef;
}

void Collection::fill(void* value) {
	apply(
		[](void* item, Key key, UINT32 ix, Collection* collection, void* arg) {
			collection->set(item, arg);
			return 1;
		}
	);
}

void* Collection::search(Key key, int& ix, COLLECTIONCALLBACK* compare) {
	void* value = NULL;
	void* args[4] = { &key, &ix, compare, &value };
	apply(
		[](void* item, Key key, UINT32 ix, Collection* collection, void* args) {
			var args_ = (void**)args;
			Key key_ = *(Key*)(args_[0]);
			*(UINT32*)(args_[1]) = ix;
			var compare_ = (COLLECTIONCALLBACK*)((void**)args_)[2];
			int res = compare_(item, key, ix, collection, NULL);
			if (res == 0) {
				*(void**)(args_[3]) = item;
			}
			return res;
		}, &args);
	return value;
}