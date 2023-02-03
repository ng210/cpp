//*****************************************************************************
// SArray - static array
//*****************************************************************************

#include <string.h>
#include <stdarg.h>
#include "collection/array.h"
#include "base/memory.h"
#include "base/str.h"
#include "base/string.h"
#include "utils/utils.h"

NS_FW_BASE_USE

Array::Array() {
	capacity_ = ARRAY_DEFAULT_CAPACITY;
	extendSize_ = ARRAY_DEFAULT_CAPACITY;
	data_ = NULL;
}

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
		data_ = (void**)MALLOC(char, (size_t)capacity_ * itemSize);
		length_ = 0;
	}
}
void Array::clear() {
	length_ = 0;
}

#pragma region Collection
void* Array::get(Key key) {
	int i = key.i;
	if (i < 0) i += length_;
	return i < length_ ? (void*)&((char*)data_)[i * itemSize_] : NULL;
}
void* Array::insert(Key key, void* item) {
	int i = key.i;
	char* dst = NULL;
	if (i >= 0 && i <= length_) {
		if (length_ == capacity_) {
			// increase capacity
			capacity_ += extendSize_;
			// realloc data
			data_ = (void**)REALLOC(data_, char, (size_t)capacity_ * itemSize_);
		}
		dst = &((char*)data_)[i * itemSize_];
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
void Array::remove(Key key) {
	int i = key.i;
	if (i < 0) i += length_;
	if (i < length_) {
		// shift items beyond ix down
		char* dst = &((char*)data_)[i * itemSize_];
		length_--;
		memcpy(dst, dst + itemSize_, (size_t)(length_ - i) * itemSize_);
		if ((UINT32)length_ < capacity_ - extendSize_) {
			// decrease capacity
			capacity_ -= extendSize_;
			data_ = (void**)REALLOC(data_, char, (size_t)capacity_ * itemSize_);
		}
	}
}
void Array::set(Key key, void* item) {
	void* slot = get(key);
	if (slot != NULL && item != NULL) {
		set((void**)slot, item);
	}
}
void Array::set(void** pOldItem, void* newItem) {
	memcpy(pOldItem, newItem, itemSize_);
}

int Array::apply(COLLECTIONCALLBACK* callback, ...) {
	va_list args;
	va_start(args, callback);
	var value = data_;
	var res = -1;
	for (var ix = 0; ix < length_; ix++) {

		if (!callback(value, NULL, ix, this, args)) {
			res = ix;
			break;
		}
		value = (void**)((byte*)value +itemSize_);
	}
	va_end(args);
	return res;
}
void Array::fill(void* value) {
	apply([](void* item, Key key, UINT32 ix, Collection* collection, void* args) {
		var source = args;
		collection->set(item, source);
		return 1;
	}, value);
}
#pragma endregion

int Array::join(ArrayBase* array) {
	int res = -1;
	UINT32 len = length_;
	length_ = length_ + array->length();
	if (itemSize_ == array->itemSize()) {
		if ((UINT32)length_ > capacity_) {
			capacity_ = (length_ / extendSize_ + 1) * extendSize_;
			data_ = (void**)REALLOC(data_, char, (size_t)capacity_ * itemSize_);
		}
		memcpy(&((char*)data_)[len*itemSize_], array->data(), (size_t)array->length() * array->itemSize());
		res = length_;
	}
	return res;
}

void Array::sort_(int min, int max, COLLECTIONCALLBACK* compare) {
	if (min < max) {
		// create a random index
		int ix = min;	// (int)(min + Utils::random(max - min));
		int l = min, r = max;
		void* pivot = get(ix);
		void* tmp = MALLOC(char, itemSize_);
		while (l <= r) {
			void* item;
			while ((item = get(l)) != NULL && compare(item, pivot, l, this, NULL) < 0) l++;
			while ((item = get(r)) != NULL && compare(item, pivot, r, this, NULL) > 0) r--;
			if (l > r) break;
			// swap left and right
			void* left = get(l);
			void* right = get(r);
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
//void* Array::binSearch_(void* key, int& ix, COLLECTIONCALLBACK* compare) {
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
void* Array::search(Key key, int& ix, COLLECTIONCALLBACK* compare) {
	void* value = NULL;
	if (compare == NULL) compare = compare_;
	void* item = get(0);
	for (int i = 0; i < length_; i++) {
		int res = compare(item, key, i, this, NULL);
		if (res == 0) {
			ix = i;
			value = item;
			break;
		}
		item = (void*)((char*)item + itemSize_);
	}
	return value;
}
char* Array::str_join(const char* filler) {
	char** parts = MALLOC(char*, (size_t)length_ + 1);
	for (int i = 0; i < (int)length_; i++) {
		parts[i] = &((char*)data_)[i*itemSize_];
	}
	parts[length_] = NULL;
	char* str = NS_FW_BASE::str_join(parts, filler);
	FREE(parts);
	return str;
}
