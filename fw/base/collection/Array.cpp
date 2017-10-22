#include "collection/Array.h"
#include "base/MemoryMgr.h"
#include "base/str.h"

#include <stdio.h>

NS_FW_BASE_BEGIN

#define ARRAY_BLOCK_SIZE 16

/*****************************************************************************
* Array
*****************************************************************************/
Array::Array(void) {
	init(ARRAY_BLOCK_SIZE);
}
Array::Array(size_t count) {
	init(count);
}
Array::Array(size_t count, Object* obj, ...) {
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
void Array::init(size_t count) {
	length_ = 0;
	size_t mod = count / ARRAY_BLOCK_SIZE;
	capacity_ = ARRAY_BLOCK_SIZE * (mod+1);
	data_ = (Object**)MALLOC(Object*, capacity_);
}
/*****************************************************************************
* Object
*****************************************************************************/
const char* Array::getType() {
	return "array";
}
int Array::compareTo(Object* obj) {
	throw "Not implemented!";
}
const char* Array::toString() {
	return join("")->toString();
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
Array* Array::concat(Array* arr) {
	Array* res = NEW_(Array, length_ + arr->length_);
	// add items of this
	res->push(length_, data_);
	// add items of arr
	res->push(arr->length_, arr->data_);
	return res;
}
void Array::fill(Object* obj, size_t start, size_t end) {
	if (start < length_) {
		if (end == 0) {
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
		if (*(bool*)(callback(4, that, data_[i], i, *this)->valueOf()) == true) {
			arr->push(data_[i]);
		}
	}
	return arr;
}
Object* Array::find(Function* callback, Object* that) {
	Object* res;
	for (size_t i = 0; i < length_; i++) {
		// bool callback(currentValue, index, arr)
		//if (*(bool*)(callback(4, that, data_[i], i, *this)->valueOf()) == true) {
		if (*((bool*)callback(4, that, data_[i], i, *this)->valueOf()) == true) {
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
		if (*(bool*)(callback(4, that, data_[i], i, *this)->valueOf()) == true) {
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
long long Array::indexOf(Object* obj, size_t start) {
	long long res = -1;
	if (start < 0) {
		start += length_;
	}
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
	return NEW_(String, join_(sep));
}
const char* Array::join_(const char* sep) {
	size_t size = 65536;
	char *buffer = MALLOC(char, size);
	size_t sepLen = strlen(sep);
	size_t offset = 0;
	for (size_t i = 0; i < length_; i++) {
		Object* o = data_[i];
		const char* item = o->toString();
		size_t len = strlen(item);
		strnncpy(buffer, size, offset, item, len);
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
long long Array::lastIndexOf(Object* obj, size_t start) {
	long long res = -1;
	if (start < 0) {
		start += length_;
	}
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
	return push(count, args);
}
size_t Array::push(size_t count, va_list args) {
	if (count > 0) {
		size_t len = length_ + count;
		if (len >= capacity_) {
			capacity_ += ALIGN(len, ARRAY_BLOCK_SIZE);
			data_ = REALLOC(data_, Object*, capacity_);
		}
		for (size_t i = 0; i < count; i++) {
			Object* item = (*(Object***)args)[i];
			data_[length_++] = item;
		}
	}
	return length_;
}
Array* Array::reverse() {
	throw "Not implemented!";
}
Object* Array::shift() {
	Object* res;
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
		arr = NEW_(Array, end - start, data_[start]);
	}
	return arr;
}
void Array::sort(Function* callback) {
	throw "Not implemented!";
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
	if (index < (long long)length_) {
		Object* p = data_[index];
		// remove old entries
		arr = NEW_(Array, count1, p);
		size_t diff = count2 - count1;
		if (diff > 0) {
			// make room for new entries
			if (length_ + diff > capacity_) {
				capacity_ += ARRAY_BLOCK_SIZE;
				data_ = REALLOC(data_, Object*, capacity_);
			}
			for (size_t i = length_ - 1; i >= index + count1; i--) {
				data_[i + diff] = data_[i];
			}
		}
		// add new entries
		for (size_t i = 0; i < count2; i++) {
			data_[i + index] = va_arg(args, Object*);
			length_++;
		}
		diff = count1 - count2;
		if (diff > 0) {
			// shift old elements down
			for (size_t i = index + count2; i < length_; i++) {
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
		for (size_t i = length_ - 1; i >= 0; i--) {
			data_[i + count] = data_[i];
		}
		// add new entries
		for (size_t i = 0; i < count; i++) {
			data_[i] = va_arg(args, Object*);
			length_++;
		}
	}
	return length_;
}

NS_FW_BASE_END
