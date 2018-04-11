#include "collection/arraybase.h"
#include "base/memory.h"

NS_FW_BASE_BEGIN

ArrayBase::~ArrayBase() {
	FREE(data_);
}

void* ArrayBase::binSearch(void* key, int& ix, CollectionCallback* compare) {
	if (compare == NULL) compare = compare_;
	return binSearch_(key, ix, compare);
}
void* ArrayBase::binSearch_(void* key, int& ix, CollectionCallback* compare) {
	UINT32 min = 0, max = length_;
	// the search range gets halved each iteration
	while (min < max) {
		// mid = min + (max - min)/2 = min/2 + max/2
		UINT32 mid = (min + max) >> 1;
		// compare item with middle item
		void* item = getAt(mid);
		int i = compare(item, mid, this, key);
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
int ArrayBase::findIndex(void* key, CollectionCallback* compare) {
	if (compare == NULL) compare = compare_;
	return forEach(compare, key);
}
void ArrayBase::sort(CollectionCallback* compare) {
	if (compare == NULL) compare = compare_;
	return sort_(0, length_ - 1, compare);
}

NS_FW_BASE_END