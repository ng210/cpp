//*****************************************************************************
// PArray - pointer array
//*****************************************************************************

#include "collection/parray.h"
#include "base/memory.h"
#include "base/str.h"
#include "base/string.h"
#include "utils/utils.h"
#include <string.h>
#include <stdarg.h>

NS_FW_BASE_BEGIN

#define ITEMSIZE sizeof(void*)

PArray::PArray(UINT32 capacity) {
	data_ = NULL;
	init(capacity);
}
//PArray::~PArray() {
//}
void PArray::init(UINT32 capacity, void* items) {
	if (items == NULL) {
		if (data_ != NULL) {
			FREE(data_);
		}
		capacity_ = capacity;
		extendSize_ = capacity;
		data_ = MALLOC(void*, capacity_);
		length_ = 0;
	}
	itemSize_ = sizeof(void*);
}
void* PArray::add(void* item) {
	// insert item at the end
	return insertAt(length_, item);
}
void* PArray::insertAt(int ix, void* item) {
	int i = ix < 0 ? ix + length_ : ix;
	if (i <= length_) {
		if (length_ == capacity_) {
			// increase capacity
			capacity_ += extendSize_;
			// realloc data
			data_ = REALLOC(data_, void*, capacity_);
		}
		// shift items beyond i up
		for (int size = (int)length_; size > (int)i; size--) {
			data_[size] = data_[size - 1];
		}
		data_[i] = item;
		length_++;
	}
	return item;
}
void PArray::removeAt(int ix) {
	int i = ix < 0 ? ix + length_ : ix;
	if (i < length_) {
		// shift items beyond i down
		length_--;
		memcpy(&data_[i], &data_[i + 1], (length_ - i) * ITEMSIZE);
		if ((UINT32)length_ < capacity_ - extendSize_) {
			// decrease capacity
			capacity_ -= extendSize_;
			data_ = REALLOC(data_, void*, capacity_);
		}
	}
}
void* PArray::getAt(int ix) {
	int i = ix < 0 ? ix + length_ : ix;
	return i < length_ ? (void*)data_[i] : NULL;
}
void PArray::setAt(int ix, void* item) {
	int i = ix < 0 ? ix + length_ : ix;
	if (i < length_) {
		data_[i] = item;
	}
}
int PArray::join(ArrayBase* array) {
	int len = length_;
	length_ = length_ + array->length();
	if ((UINT32)length_ > capacity_) {
		capacity_ = (length_ / extendSize_ + 1) * extendSize_;
		data_ = REALLOC(data_, void*, capacity_);
	}
	memcpy(&data_[len], array->data(), array->length()*ITEMSIZE);
	return length_;
}
int PArray::forEach(CollectionCallback* action, void* args) {
	int i = 0;
	int hasFound = 0;
	while (i < length_) {
		if (!action(data_[i], i, this, args)) {
			hasFound = true;
			break;
		}
		i++;
	}
	return hasFound ? i : -1;
}
void PArray::sort_(int min, int max, CollectionCallback* compare) {
	if (min < max) {
		// create a random index
		int ix = min;
		int l = min, r = max;
		void* pivot = getAt(ix);
		while (l <= r) {
			void* item;
			while ((item = getAt(l)) != NULL && compare(item, l, this, pivot) < 0) l++;
			while ((item = getAt(r)) != NULL && compare(item, r, this, pivot) > 0) r--;
			if (l > r) break;
			// swap left and right
			void* tmp = data_[l];
			data_[l] = data_[r];
			data_[r] = tmp;
			l++, r--;
		}
		sort_(min, r, compare);
		sort_(l, max, compare);
	}
}
//void* PArray::binSearch_(void* key, int& ix, CollectionCallback* compare) {
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
void* PArray::search(void* key, int& ix, CollectionCallback* compare) {
	void* value = NULL;
	if (compare == NULL) compare = this->compare();
	for (int i = 0; i < length_; i++) {
		void* item = data_[i];
		int res = compare(item, i, this, key);
		if (res == 0) {
			value = item;
			ix = i;
			break;
		}
		//if (res > 0) {
		//	ix = i;
		//	break;
		//}
	}
	return value;
}
char* PArray::str_join(const char* filler) {
	char** parts = MALLOC(char*, (size_t)length_ + 1);
	memcpy(parts, data_, sizeof(char*) * length_);
	parts[length_] = NULL;
	char* str = NS_FW_BASE::str_join(parts, filler);
	FREE(parts);
	return str;
}

PArray* PArray::str_split(const char* src, const char* str) {
	PArray* arr = NEW_(PArray);
	int i = 0;
	char* pos = (char*)src;
	while (*pos != '\0') {
		char* start = pos;
		while (*pos != '\0' && strchr(str, *pos) == NULL) pos++;
		if (start != pos) {
			char* token = substr(start, 0, pos - start);
			arr->add(token);
		}
		while (*pos != '\0' && strchr(str, *pos) != NULL) pos++;
	}
	return arr;
}

NS_FW_BASE_END