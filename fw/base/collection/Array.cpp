#include "collection/Array.h"
#include "base/MemoryMgr.h"
#include "base/str.h"
#include "base/utils.h"

#include <stdio.h>

NS_FW_BASE_BEGIN

#define ARRAY_BLOCK_SIZE 16

const Type* Array::classType_;

void Array::initialize() {
	// add type entry
	classType_ = Type::add(STRINGIFY(NS_FW_BASE)"::array");
}
void Array::shutDown() {
}
int Array::compare(Object* a, Object* b) {
	return a->compareTo(b);
}
/*****************************************************************************
* Array
*****************************************************************************/
Array::Array(void) {
	data_ = NULL;
	init(ARRAY_BLOCK_SIZE);
}
Array::Array(size_t count) {
	data_ = NULL;
	init(count);
}
Array::Array(size_t count, Object* obj, ...) {
	data_ = NULL;
	init(count);
	va_list items;
	va_start(items, count);
	memcpy((char*)data_, (char*)items, count*sizeof(Object*));
	length_ += count;
}
//Array::Array(size_t count, Object** items) {
//	init(count);
//	memcpy((char*)data_, (char*)items, sizeof(Object*));
//	length_ += count;
//}
Array::~Array(void) {
	//for (size_t i = 0; i < length_; i++) {
	//	DEL_(data_[i]);
	//}
	FREE(data_);
}
void Array::init(size_t count, va_list items) {
	length_ = 0;
	size_t mod = count / ARRAY_BLOCK_SIZE;
	capacity_ = ARRAY_BLOCK_SIZE * (mod+1);
	//data_ = (Object**)
	data_ = REALLOC(data_, Object*, capacity_);
	if (items != NULL) {
		for (size_t i = 0; i < count; i++) {
			data_[i] = va_arg(items, Object*);
			length_++;
		}
	}
}
/*****************************************************************************
* Object
*****************************************************************************/
int Array::compareTo(Object* obj) {
	throw "Not implemented!";
}
char* Array::toString() {
	return join_(",");
}
void* Array::valueOf() {
	return data_;
}
/*****************************************************************************
* Methods
*****************************************************************************/
Object* Array::operator[](long long index) {
	Object* item = NULL;
	if (index < 0) {
		index += length_;
	}
	if (index < (long long)length_) {
		item = data_[index];
	}

	return item;
}
void Array::cleanUp() {
	for (int i = 0; i < length_; i++) {
		DEL_(data_[i]);
	}
	length_ = 0;
}
Array* Array::concat(Array* arr) {
	Array* res = NEW_(Array, length_ + arr->length_);
	// add items of this
	res->push_(length_, (va_list)data_);
	// add items of arr
	res->push_(arr->length_, (va_list)arr->data_);
	return res;
}
void Array::fill(Object* obj, size_t start, size_t end) {
	if (start < length_) {
		if (end == 0 || end > length_) {
			end = length_;
		}
		for (size_t i = start; i < end; i++) {
			data_[i] = obj;
		}
	}
	return;
}
Array* Array::filter(Function* callback, Object* that) {
	Array* arr = NEW_(Array);
	for (size_t i = 0; i < length_; i++) {
		// bool callback(currentValue, index, arr)
		if (callback(4, that, data_[i], i, *this)->toBool()) {
			arr->push(data_[i]);
		}
	}
	return arr;
}
Object* Array::find(Function* callback, Object* that) {
	Object* res;
	for (size_t i = 0; i < length_; i++) {
		// bool callback(currentValue, index, arr)
		if (callback(4, that, data_[i], i, *this)->toBool()) {
			res = data_[i];
			break;
		}
	}
	return res;
}
long long Array::findIndex(Function* callback, Object* that) {
	size_t res = -1;
	for (size_t i = 0; i < length_; i++) {
		// bool callback(currentValue, index, arr)
		if (callback(4, that, data_[i], i, *this)->toBool()) {
			res = i;
			break;
		}
	}
	return res;
}
void Array::forEach(Function* callback, Object* that) {
	for (size_t i = 0; i < length_; i++) {
		// bool callback(currentValue, index, arr)
		callback(4, that, data_[i], i, this);
	}
}
long long Array::indexOf(Object* obj, long long start) {
	long long res = -1;
	if (start < 0) start += length_;
	if (start < 0) start = 0;
	else if (start >(long long)length_) start = length_;
	for (size_t i = start; i < length_; i++) {
		if (data_[i]->compareTo(obj) == 0) {
			res = i;
			break;
		}
	}
	return res;
}
String* Array::join(String* str) {
	return join(str->toString());
}
String* Array::join(const char* sep) {
	return NEW_(String, (char*)join_(sep));
}
char* Array::join_(const char* sep) {
	size_t size = 65536;
	char *buffer = MALLOC(char, size);
	size_t sepLen = strlen(sep);
	size_t offset = 0;
	for (size_t i = 0; i < length_; i++) {
		Object* o = data_[i];
		char* item = o->toString();
		size_t len = strlen(item);
		strnncpy(buffer, size, offset, item, len);
		FREE(item);
		offset += len;
		if (i < length_ - 1) {
			strnncpy(buffer, size, offset, sep, sepLen);
			offset += sepLen;
		}
	}
	// realloc buffer
	buffer[offset] = '\0';
	offset++;
	buffer = REALLOC(buffer, char, offset);
	return buffer;
}
long long Array::lastIndexOf(Object* obj, long long start) {
	long long res = -1;
	if (start < 0) start += length_;
	if (start < 0) start = 0;
	else if (start > (long long)length_) start = length_;
	for (long long i = start; i >= 0; i--) {
		if (data_[i]->compareTo(obj) == 0) {
			res = i;
			break;
		}
	}
	return res;
}
Array* Array::map(Function* callback, Object* that) {
	Array* arr = NEW_(Array);
	for (size_t i = 0; i < length_; i++) {
		// bool callback(currentValue, index, arr)
		arr->push((Object*)callback(4, that, data_[i], i, *this));
	}
	return arr;
}
Object* Array::pop() {
	Object* res = NULL;
	if (length_ > 0) {
		res = data_[--length_];
		if (length_ < capacity_ - ARRAY_BLOCK_SIZE) {
			capacity_ -= ARRAY_BLOCK_SIZE;
			data_ = REALLOC(data_, Object*, capacity_);
		}
	}
	return res;
}
size_t Array::push(Object* item) {
	if (length_ == capacity_) {
		capacity_ += ARRAY_BLOCK_SIZE;
		data_ = REALLOC(data_, Object*, capacity_);
	}
	data_[length_++] = item;
	return length_;
}
size_t Array::push(size_t count, ...) {
	va_list args;
	va_start(args, count);
	return push_(count, args);
}
size_t Array::push_(size_t count, va_list args) {
	if (count > 0) {
		size_t len = length_ + count;
		if (len >= capacity_) {
			capacity_ += ALIGN(len, ARRAY_BLOCK_SIZE);
			data_ = REALLOC(data_, Object*, capacity_);
		}
		for (size_t i = 0; i < count; i++) {
			Object* item = ((Object**)args)[i];
			data_[length_++] = item;
		}
	}
	return length_;
}
Array* Array::reverse() {
	size_t i = 0, j = length_ - 1;
	while (i < j) {
		// Exchange items at i and j
		Object* tmp = data_[i];
		data_[i] = data_[j];
		data_[j] = tmp;
		i++; j--;
	}
	return this;
}
Object* Array::shift() {
	Object* res = Null;
	size_t len = length_;
	if (len > 0) {
		res = data_[0];
		for (size_t i = 1; i < len; i++) {
			data_[i - 1] = data_[i];
		}
		length_ = len - 1;
	}
	return res;
}
Array* Array::slice(long long start, long long end) {
	Array* arr = NEW_(Array);
	if (start < 0) start += length_;
	if (end < 0) end += length_;
	if (start > end) {
		long long tmp = start;
		start = end;
		end = tmp;
	}
	if (start < (long long)length_ && end < (long long)length_) {
		arr->init(end - start + 1, (va_list)&data_[start]);
	}
	return arr;
}
void Array::sort(long long min, long long max, Compare* compare) {
	if (min < max) {
		// create a random index
		size_t ix = min + Utils::random((size_t)(max - min));
		// get the pivot element
		Object* pivot = data_[ix];
		// move the last element to the pivot's place
		data_[ix] = data_[max];
		// check elements against the pivot
		size_t j = min;
		for (size_t i = (size_t)min; i < (size_t)max; i++) {
			if (compare(data_[i], pivot) < 0) {
				// swap this[i] and this[j]
				Object* tmp = data_[j];
				data_[j] = data_[i];
				data_[i] = tmp;
				j++;
			}
		}
		data_[max] = data_[j];
		data_[j] = pivot;

		sort(min, j - 1, compare);
		sort(j + 1, max, compare);
	}
}
void Array::sort(Compare* compare) {
	sort(0, length_ - 1, compare);
}
Array* Array::splice(long long index, size_t count1, size_t count2, ...) {
	va_list args;
	va_start(args, count2);
	return splice(index, count1, count2, args);
}
Array* Array::splice(long long index, size_t count1, size_t count2, va_list args) {
	Array* arr = NULL;
	if (index < 0) {
		index += length_;
	}
	size_t ix = (size_t)index;
	if (ix < length_) {
		// remove old entries
		arr = NEW_(Array, count1);
		memcpy((char*)arr->data_, (char*)&data_[ix], count1*sizeof(Object*));
		arr->length_ = count1;

		if (count2 > count1) {
			size_t diff = count2 - count1;
			// make room for new entries
			if (length_ + diff > capacity_) {
				capacity_ += ARRAY_BLOCK_SIZE;
				data_ = REALLOC(data_, Object*, capacity_);
			}
			for (size_t i = length_ - 1; i >= ix + count1; i--) {
				data_[i + diff] = data_[i];
			}
		}
		// add new entries
		for (size_t i = 0; i < count2; i++) {
			data_[i + ix] = va_arg(args, Object*);
			length_++;
		}
		
		if (count1 > count2) {
			size_t diff = count1 - count2;
			// shift old elements down
			for (size_t i = ix + count2; i < length_; i++) {
				data_[i] = data_[i + diff];
			}
			length_ -= diff;
			if (length_ < capacity_ - ARRAY_BLOCK_SIZE) {
				capacity_ -= ARRAY_BLOCK_SIZE;
				data_ = REALLOC(data_, Object*, capacity_);
			}
		}
	}
	return arr;
}
size_t Array::unshift(Object* item) {
	return unshift(1, item);
}
size_t Array::unshift(size_t count, ...) {
	va_list args;
	va_start(args, count);
	return unshift(count, args);
}
size_t Array::unshift(size_t count, va_list args) {
	if (count > 0) {
		size_t len = length_ + count;
		if (len >= capacity_) {
			capacity_ += ALIGN(len, ARRAY_BLOCK_SIZE);
			data_ = REALLOC(data_, Object*, capacity_);
		}
		// make room for new entries
		for (long long i = length_ - 1; i >= 0; i--) {
			data_[i + count] = data_[i];
		}
		// add new entries
		size_t j = count - 1;
		for (size_t i = 0; i < count; i++) {
			data_[j] = va_arg(args, Object*);
			j--;
			length_++;
		}
	}
	return length_;
}

NS_FW_BASE_END
