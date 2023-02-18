//*****************************************************************************
// PArray - pointer array
//*****************************************************************************

#include <string.h>
#include <stdarg.h>
#include "base/memory.h"
#include "base/str.h"
#include "base/string.h"
#include "collection/parray.h"
#include "utils/utils.h"

NS_FW_BASE_USE

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

#pragma region Collection
void* PArray::add(Key key, void* item) {
	return insert(key, item);
}
void* PArray::get(Key key) {
	int i = key.i;
	if (i < 0) i += length_;
	return i < length_ ? (void*)data_[i] : NULL;
}
void* PArray::insert(Key key, void* item) {
	int i = key.i;
	if (i >= 0 && i <= length_) {
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
void PArray::remove(Key key) {
	int i = key.i;
	if (i < 0) i += length_;
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
void PArray::set(Key key, void* item) {
	int i = key.i;
	if (i < 0) i += length_;
	if (i < length_) {
		data_[i] = item;
	}
}
void PArray::set(void** pOldItem, void* newItem) {
	*pOldItem = newItem;
}

int PArray::apply(COLLECTION_ACTION* action, ...) {
	va_list args;
	va_start(args, action);
	var res = -1;
	for (var ix = 0; ix < length_; ix++) {
		if (!action(data_[ix], ix, this, args)) {
			res = ix;
			break;
		}
	}
	va_end(args);
	return res;
}
void PArray::fill(void* value) {
	for (var i = 0; i < length_; i++) {
		data_[i] = value;
	}
}
#pragma endregion

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

void PArray::sort_(int min, int max, COLLECTION_COMPARE* compare) {
	if (min < max) {
		// create a random index
		int ix = min;
		int l = min, r = max;
		void* pivot = get(ix);
		while (l <= r) {
			void* item;
			while ((item = get(l)) != NULL && compare(item, pivot, this, NULL) < 0) l++;
			while ((item = get(r)) != NULL && compare(item, pivot, this, NULL) > 0) r--;
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
void* PArray::search(Key key, Key& found, COLLECTION_COMPARE* compare) {
	void* value = NULL;
	if (compare == NULL) compare = this->compare();
	for (int i = 0; i < length_; i++) {
		void* item = data_[i];
		int res = compare(item, key, this, NULL);
		if (res == 0) {
			value = item;
			found.i = i;
			break;
		}
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
PArray* PArray::splice(Key pos, int count) {
	var arr = NEW_(PArray, count);
	var ptr = &data_[pos.i];
	// copy data into new array
	fmw::memcpy(arr->data_, ptr, ITEMSIZE * count);
	arr->length_ = count;
	// shift elements down
	var p2 = pos.i + count;
	if (p2 < length_) {
		memcpy(&data_[pos.i], &data_[p2], (length_ - p2) * ITEMSIZE);
	}
	length_ -= count;
	if ((UINT32)length_ < capacity_ - extendSize_) {
		// decrease capacity
		capacity_ -= extendSize_;
		data_ = REALLOC(data_, void*, capacity_);
	}
	return arr;
}

PArray* PArray::map(COLLECTION_ACTION* action) {
	var arr = NEW_(PArray, length_);
	for (var i = 0; i < length_; i++) {
		var value = action(data_[i], i, this, NULL);
		arr->push(value);
	}
	return arr;
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
			arr->push(token);
		}
		while (*pos != '\0' && strchr(str, *pos) != NULL) pos++;
	}
	return arr;
}
