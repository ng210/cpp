//*****************************************************************************
// SArray - static array
//*****************************************************************************

#include "collection/array.h"
#include "base/memory.h"
#include "base/str.h"
#include "utils/utils.h"
#include <string.h>
#include <stdarg.h>

NS_FW_BASE_BEGIN

Array::Array(UINT32 itemSize, UINT32 capacity) {
	data_ = NULL;
	init(itemSize, capacity);
}
Array::~Array() {
	FREE(data_);
}
void Array::init(UINT32 itemSize, UINT32 capacity, void* items) {
	if (items == NULL) {
		if (data_ != NULL) {
			FREE(data_);
		}
		itemSize_ = itemSize;
		capacity_ = capacity;
		extendSize_ = capacity;
		data_ = MALLOC(char, capacity_ * itemSize);
		length_ = 0;
	}
}
void* Array::add(void* item) {
	// insert item at the end
	return insertAt(length_, item);
}
void* Array::insertAt(int ix, void* item) {
	UINT32 i = ix < 0 ? ix + length_ : ix;
	char* dst = NULL;
	if (i <= length_) {
		if (length_ == capacity_) {
			// increase capacity
			capacity_ += extendSize_;
			// realloc data
			data_ = REALLOC(data_, char, capacity_ * itemSize_);
		}
		dst = &data_[i*itemSize_];
		// shift items beyond ix up
		int size = (length_ - i) * itemSize_;
		if (size > 0) {
			char* tmp = MALLOC(char, size);
			memcpy(tmp, dst, size);
			memcpy(dst + itemSize_, tmp, size);
			FREE(tmp);
		}
		memcpy(dst, item, itemSize_);
		length_++;
	}
	return (void*)dst;
}
void Array::removeAt(int ix) {
	UINT32 i = ix < 0 ? ix + length_ : ix;
	if (i < length_) {
		// shift items beyond ix down
		char* dst = &data_[i*itemSize_];
		length_--;
		memcpy(dst, dst + itemSize_, (length_ - i) * itemSize_);
		if (length_ < capacity_ - extendSize_) {
			// decrease capacity
			capacity_ -= extendSize_;
			data_ = REALLOC(data_, char, capacity_ * itemSize_);
		}
	}
}
void* Array::getAt(int ix) {
	UINT32 i = ix < 0 ? ix + length_ : ix;
	return i < length_ ? (void*)&data_[i*itemSize_] : NULL;
}
void* Array::setAt(int ix, void* item) {
	void* slot = getAt(ix);
	if (slot != NULL && item != NULL) {
		memcpy(slot, item, itemSize_);
	}
	return (void*)slot;
}
int Array::join(Array* array) {
	int res = -1;
	UINT32 len = length_;
	length_ = length_ + array->length_;
	if (itemSize_ == array->itemSize_) {
		if (length_ > capacity_) {
			capacity_ = (length_ / extendSize_ + 1) * extendSize_;
			data_ = REALLOC(data_, char, capacity_ * itemSize_);
		}
		memcpy(&data_[len*itemSize_], array->data_, array->length_*array->itemSize_);
		res = length_;
	}
	return res;
}
int Array::forEach(CollectionCallback* action, void* args) {
	void* item = getAt(0);
	UINT32 i = 0;
	int hasFound = 0;
	while (i < length_) {
		if (!action(item, i, this, args)) {
			hasFound = true;
			break;
		}
		item = (void*)((char*)item + itemSize_);
		i++;
	}
	return hasFound ? i : -1;
}
int Array::findIndex(void* key, CollectionCallback* compare) {
	if (compare == NULL) compare = compare_;
	return forEach(compare, key);
}
void Array::sort_(int min, int max, CollectionCallback* compare) {
	if (min < max) {
		// create a random index
		int ix = min;	// (int)(min + Utils::random(max - min));
		int l = min, r = max;
		void* pivot = getAt(ix);
		void* tmp = MALLOC(char, itemSize_);
		while (l <= r) {
			while (compare(getAt(l), l, this, pivot) < 0) l++;
			while (compare(getAt(r), r, this, pivot) > 0) r--;
			if (l > r) break;
			// swap left and right
			void* left = getAt(l);
			void* right = getAt(r);
			memcpy(tmp, left, itemSize_);
			memcpy(left, right, itemSize_);
			memcpy(right, tmp, itemSize_);
			l++, r--;
		}
		FREE(tmp);
		sort_(min, r, compare);
		sort_(l, max, compare);
	}
}
void Array::sort(CollectionCallback* compare) {
	if (compare == NULL) compare = compare_;
	return sort_(0, length_ - 1, compare);
}
int Array::binSearch(void* key, int& ix, CollectionCallback* compare) {
	if (compare == NULL) compare = compare_;
	return binSearch_(key, ix, compare);
}
int Array::binSearch_(void* key, int& ix, CollectionCallback* compare) {
	long long res = 0;
	UINT32 min = 0, max = length_;
	// the search range gets halved each iteration
	while (min < max) {
		// mid = min + (max - min)/2 = min/2 + max/2
		UINT32 mid = (min + max) >> 1;
		// compare item with middle item
		int i = compare(getAt(mid), mid, this, key);

		if (i == 0) {
			// on equation the middle item was the searched item
			ix = mid;
			return 1;
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
	return 0;
}
int Array::search(void* key, int& ix, CollectionCallback* compare) {
	int isFound = 0;
	if (compare == NULL) compare = compare_;
	void* item = getAt(0);
	for (UINT32 i = 0; i < length_; i++) {
		int res = compare(item, i, this, key);
		if (res == 0) {
			isFound = ix = i;
			break;
		}
		if (res > 0) {
			ix = i;
			break;
		}
		item = (void*)((char*)item + itemSize_);
	}
	return isFound;
}

Array* Array::strsplit(const char* src, const char* str) {
	Array* arr = NEW_(Array, sizeof(char*));
	int i = 0;
	char* pos = (char*)src;
	char* start = NULL;
	while (*pos != '\0') {
		while (*pos != '\0' && strchr(str, *pos) != NULL) pos++;
		start = pos;
		while (*pos != '\0' && strchr(str, *pos) == NULL) pos++;
		if (start != pos) {
			char* token = substr(start, 0, pos - start);
			arr->add(&token);
		}
	}
	if (start != pos) {
		char* token = substr(start, 0, pos - start);
		arr->add(&token);
	}
	return arr;
}

NS_FW_BASE_END