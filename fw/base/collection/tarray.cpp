#include "collection/tarray.h"
#include "base/memory.h"
//#include "utils/utils.h"
//#include <string.h>
//#include <stdarg.h>

NS_FW_BASE_BEGIN

template<class T> TArray<T>::TArray(unsigned int capacity) {
	data_ = NULL;
	init(capacity);
}
template<class T> TArray<T>::~TArray() {
	if (data_ != NULL) {
		FREE(data_);
	}
}

template<class T> void TArray<T>::init(unsigned int capacity, T* items) {
	if (items == NULL) {
		if (data_ != NULL) {
			FREE(data_);
		}
		capacity_ = capacity;
		extendSize_ = capacity;
		data_ = MALLOC(T, capacity_ * itemSize);
		length_ = 0;
	}
}
template<class T> int TArray<T>::add(T item) {
	// insert item at the end
	return insertAt(length_, item);
}
template<class T> int TArray<T>::insertAt(int ix, T items) {
	unsigned int i = ix < 0 ? ix + length_ : ix;
	char* dst = NULL;
	if (i <= (unsigned int)length_) {
		if (length_ == capacity_) {
			// increase capacity
			capacity_ += extendSize_;
			// realloc data
			data_ = REALLOC(data_, T, capacity_);
		}
		dst = &data_[i];
		// shift items beyond ix up
		int size = (length_ - i) * sizeof(T);
		char* tmp = MALLOC(char, size);
		memcpy(tmp, dst, size);
		memcpy(dst + sizeof(T), tmp, size);
		FREE(tmp);
		memcpy(dst, item, sizeof(T));
		length_++;
	}
	return length_;
}
template<class T> void TArray<T>::removeAt(int ix) {
	unsigned int i = ix < 0 ? ix + length_ : ix;
	if (i < (unsigned int)length_) {
		// shift items beyond ix down
		char* dst = &data_[i];
		length_--;
		memcpy(dst, dst + sizeof(T), (length_ - i) * sizeof(T));
		if (length_ < capacity_ - extendSize_) {
			// decrease capacity
			capacity_ -= extendSize_;
			data_ = REALLOC(data_, char, capacity_ * sizeof(T));
		}
	}
}
template<class T> T TArray<T>::getAt(int ix) {
	unsigned int i = ix < 0 ? ix + length_ : ix;
	return i < length_ ? data_[i] : NULL;
}
template<class T> T TArray<T>::setAt(int ix, T item) {
	data_[ix] = T;
	return item;
}
//int Array::join(Array* array) {
//	int res = -1;
//	unsigned int len = length_;
//	length_ = length_ + array->length_;
//	if (itemSize_ == array->itemSize_) {
//		if (length_ > capacity_) {
//			capacity_ = (length_ / extendSize_ + 1) * extendSize_;
//			data_ = REALLOC(data_, char, capacity_ * itemSize_);
//		}
//		memcpy(&data_[len*itemSize_], array->data_, array->length_*array->itemSize_);
//		res = length_;
//	}
//	return res;
//}
//int Array::forEach(CollectionCallback* action, void* args) {
//	void* item = getAt(0);
//	unsigned int i = 0;
//	int hasFound = 0;
//	while (i < length_) {
//		if (!action(item, i, this, args)) {
//			hasFound = true;
//			break;
//		}
//		item = (void*)((char*)item + itemSize_);
//		i++;
//	}
//	return hasFound ? i : -1;
//}
//int Array::findIndex(CollectionCallback* compare, void* key) {
//	return forEach(compare, key);
//}
//void Array::sort_(CollectionCallback* compare, int min, int max) {
//	if (min < max) {
//		// create a random index
//		int ix = min;	// (int)(min + Utils::random(max - min));
//		int l = min, r = max;
//		void* pivot = getAt(ix);
//		void* tmp = MALLOC(char, itemSize_);
//		while (l <= r) {
//			while (compare(getAt(l), l, this, pivot) < 0) l++;
//			while (compare(getAt(r), r, this, pivot) > 0) r--;
//			if (l > r) break;
//			// swap left and right
//			void* left = getAt(l);
//			void* right = getAt(r);
//			memcpy(tmp, left, itemSize_);
//			memcpy(left, right, itemSize_);
//			memcpy(right, tmp, itemSize_);
//			l++, r--;
//		}
//		FREE(tmp);
//		sort_(compare, min, r);
//		sort_(compare, l, max);
//	}
//}
//void Array::sort(CollectionCallback* compare) {
//	return sort_(compare, 0, length_ - 1);
//}
//int Array::binSearch(CollectionCallback* compare, int& ix, void* key) {
//	long long res = 0;
//	unsigned int min = 0, max = length_;
//	// the search range gets halved each iteration
//	while (min < max) {
//		// mid = min + (max - min)/2 = min/2 + max/2
//		unsigned int mid = (min + max) >> 1;
//		// compare item with middle item
//		int i = compare(getAt(mid), mid, this, key);
//
//		if (i == 0) {
//			// on equation the middle item was the searched item
//			ix = mid;
//			return mid;
//		}
//		if (i < 0) {
//			// item was less, continue with the first half-range: [min, mid]
//			max = mid;
//		}
//		else {
//			// item was greater, continue with the second half-range: [mid+1, max]
//			min = mid + 1;
//		}
//	}
//	// item not found, negative index marks the place where the item should be.
//	ix = max;
//	return 0;
//}
//int Array::search(CollectionCallback* compare, int& ix, void* key) {
//	int isFound = 0;
//	void* item = getAt(0);
//	for (unsigned int i = 0; i < length_; i++) {
//		int res = compare(item, i, this, key);
//		if (res == 0) {
//			isFound = ix = i;
//			break;
//		}
//		if (res > 0) {
//			ix = i;
//			break;
//		}
//		item = (void*)((char*)item + itemSize_);
//	}
//	return isFound;
//}

NS_FW_BASE_END