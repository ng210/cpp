//*****************************************************************************
// SArray - static array
//*****************************************************************************

#include "collection/array.h"
#include "base/memory.h"
#include "base/str.h"
#include "base/string.h"
#include "utils/utils.h"
#include <string.h>
#include <stdarg.h>

NS_FW_BASE_BEGIN

Array::Array(UINT32 itemSize, UINT32 capacity) {
	data_ = NULL;
	init(itemSize, capacity);
}
//Array::~Array() {
//}
void Array::init(UINT32 itemSize, UINT32 capacity, void* items) {
	if (items == NULL) {
		if (data_ != NULL) {
			FREE(data_);
		}
		itemSize_ = itemSize;
		capacity_ = capacity;
		extendSize_ = capacity;
		data_ = (void**)MALLOC(char, capacity_ * itemSize);
		length_ = 0;
	}
}
void Array::clear() {
	length_ = 0;
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
			data_ = (void**)REALLOC(data_, char, capacity_ * itemSize_);
		}
		dst = &((char*)data_)[i*itemSize_];
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
	return dst;
}
void Array::removeAt(int ix) {
	UINT32 i = ix < 0 ? ix + length_ : ix;
	if (i < length_) {
		// shift items beyond ix down
		char* dst = &((char*)data_)[i*itemSize_];
		length_--;
		memcpy(dst, dst + itemSize_, (length_ - i) * itemSize_);
		if (length_ < capacity_ - extendSize_) {
			// decrease capacity
			capacity_ -= extendSize_;
			data_ = (void**)REALLOC(data_, char, capacity_ * itemSize_);
		}
	}
}
void* Array::getAt(int ix) {
	UINT32 i = ix < 0 ? ix + length_ : ix;
	return i < length_ ? (void*)&((char*)data_)[i*itemSize_] : NULL;
}
void Array::setAt(int ix, void* item) {
	void* slot = getAt(ix);
	if (slot != NULL && item != NULL) {
		memcpy(slot, item, itemSize_);
	}
}
int Array::join(ArrayBase* array) {
	int res = -1;
	UINT32 len = length_;
	length_ = length_ + array->length();
	if (itemSize_ == array->itemSize()) {
		if (length_ > capacity_) {
			capacity_ = (length_ / extendSize_ + 1) * extendSize_;
			data_ = (void**)REALLOC(data_, char, capacity_ * itemSize_);
		}
		memcpy(&((char*)data_)[len*itemSize_], array->data(), array->length()*array->itemSize());
		res = length_;
	}
	return res;
}
int Array::forEach(CollectionCallback* action, void* args) {
	void* item = &((char*)data_)[0];
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
void Array::sort_(int min, int max, CollectionCallback* compare) {
	if (min < max) {
		// create a random index
		int ix = min;	// (int)(min + Utils::random(max - min));
		int l = min, r = max;
		void* pivot = getAt(ix);
		void* tmp = MALLOC(char, itemSize_);
		while (l <= r) {
			void* item;
			while ((item = getAt(l)) != NULL && compare(item, l, this, pivot) < 0) l++;
			while ((item = getAt(r)) != NULL && compare(item, r, this, pivot) > 0) r--;
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
//void* Array::binSearch_(void* key, int& ix, CollectionCallback* compare) {
//	UINT32 min = 0, max = length_;
//	// the search range gets halved each iteration
//	while (min < max) {
//		// mid = min + (max - min)/2 = min/2 + max/2
//		UINT32 mid = (min + max) >> 1;
//		// compare item with middle item
//		void* item = getAt(mid);
//		int i = compare(item, mid, this, key);
//		if (i == 0) {
//			// on equation the middle item was the searched item
//			ix = mid;
//			return item;
//		}
//		if (i > 0) {
//			// item was less, continue with the first half-range: [min, mid]
//			max = mid;
//		}
//		else {
//			// item was greater, continue with the second half-range: [mid+1, max]
//			min = mid + 1;
//		}
//	}
//	// item not found, index marks the place where the item should be.
//	ix = max;
//	return NULL;
//}
void* Array::search(void* key, int& ix, CollectionCallback* compare) {
	void* value = NULL;
	if (compare == NULL) compare = this->compare();
	void* item = getAt(0);
	for (UINT32 i = 0; i < length_; i++) {
		int res = compare(item, i, this, key);
		if (res == 0) {
			ix = i;
			value = item;
			break;
		}
		//if (res > 0) {
		//	ix = i;
		//	break;
		//}
		item = (void*)((char*)item + itemSize_);
	}
	return value;
}
char* Array::str_join(const char* filler) {
	char** parts = MALLOC(char*, length_ + 1);
	for (int i = 0; i < (int)length_; i++) {
		parts[i] = &((char*)data_)[i*itemSize_];
	}
	parts[length_] = NULL;
	char* str = NS_FW_BASE::str_join(parts, filler);
	FREE(parts);
	return str;
}

NS_FW_BASE_END