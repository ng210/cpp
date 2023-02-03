#include "base/memory.h"
#include "collection/arraybase.h"

NS_FW_BASE_USE

ArrayBase::ArrayBase() {
	compare_ = NULL;
	data_ = NULL;
	itemSize_ = 0;
}

ArrayBase::~ArrayBase() {
	FREE(data_);
}

void* ArrayBase::binSearch(Key key, int& ix, COLLECTIONCALLBACK* compare) {
	if (compare == NULL) compare = compare_;
	return binSearch_(key, ix, compare);
}
void* ArrayBase::binSearch_(Key key, int& ix, COLLECTIONCALLBACK* compare) {
	UINT32 min = 0, max = length_;
	// the search range gets halved each iteration
	while (min < max) {
		// mid = min + (max - min)/2 = min/2 + max/2
		UINT32 mid = (min + max) >> 1;
		// compare item with middle item
		void* item = get(mid);
		int i = compare(item, key, mid, this, NULL);
		if (i == 0) {
			// on equation the middle item was the searched item
			ix = mid;
			return item;
		}
		if (i > 0) {
			// item was less, continue with the first half-range: [min, mid]
			max = mid;
		}
		else {
			// item was greater, continue with the second half-range: [mid+1, max]
			min = mid + 1;
		}
	}
	// item not found, index marks the place where the item should be.
	ix = max;
	return NULL;
}
int ArrayBase::findIndex(Key key, COLLECTIONCALLBACK* compare) {
	if (compare == NULL) compare = compare_;
	int ix = 0;
	if (!search(key, ix, compare)) ix = -1;
	return ix;
}
void ArrayBase::sort(COLLECTIONCALLBACK* compare) {
	if (compare == NULL) compare = compare_;
	return sort_(0, length_ - 1, compare);
}

void* ArrayBase::add(Key key, void* item) {
	return insert(key, item);
}

void* ArrayBase::push(void* item) {
	return insert(length_, item);
}

void* ArrayBase::pop() {
	void* item = NULL;
	if (length_) {
		item = get(length_ - 1);
		remove(length_ - 1);
	}
	return item;
}