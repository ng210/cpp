#include "base/Array.h"
#include "base/str.h"
#include "base/String.h"
#include "base/Function.h"
#include "base/MemoryMgr.h"
#include <stdarg.h>

#include <stdio.h>

NAMESPACE_FRMWRK_USE

#define ARRAY_BLOCK_SIZE 16

/*****************************************************************************
* Object_ (structure)
*****************************************************************************/
Array_::Array_(void) {
	init_(ARRAY_BLOCK_SIZE);
}
//Array_::Array_(size_t count, ...) {
//	init_(count);
//	va_list args;
//	va_start(args, count);
//	for (size_t i = 0; i < count; i++) {
//		Object* item = va_arg(args, Object*);
//		//item->addRef_();
//		data_[i] = *item;
//	}
//	va_end(args);
//}
Array_::Array_(size_t count, Object** items) {
	init_(count);
	for (size_t i = 0; i < count; i++) {
		Object* item = items[i];
		item->addRef_();
		memcpy((char*)&data_[i], (char*)item, sizeof(Object));
		length_++;
	}
}
Array_::~Array_(void) {
	DELARR(data_);
	//for (size_t i = 0; i < length_; i++) {
	//	data_[i].delRef_();
	//}
	//FREE(data_);
}
void Array_::init_(size_t count) {
	length_ = 0;
	size_t mod = count / ARRAY_BLOCK_SIZE;
	capacity_ = ARRAY_BLOCK_SIZE * (mod+1);
	//data_ = MALLOC(Object, capacity_);
	data_ = NEWARR(Object, capacity_);
}
const char* Array_::getType() {
	return "array";
}
int Array_::compareTo(Object_* str) {
	return 0;
}
String Array_::toString() {
	return join_();
}
void* Array_::valueOf() {
	return data_;
}
String Array_::join_(String& str) {
	size_t size = 65536;
	char *buffer = MALLOC(char, size);
	size_t offset = 0;
	String_* str_ = (String_*)str.ptr_;
	for (size_t i = 0; i < length_; i++) {
		Object o = data_[i];
		String s = o.toString();
		String_* item = (String_*)s.ptr_;
		strnncpy(buffer, size, offset, item->buffer_, item->length_, 0);
		offset += item->length_;
		if (i < length_ - 1) {
			strnncpy(buffer, size, offset, str_->buffer_, str_->length_, 0);
			offset += str_->length_;
		}
	}
	// realloc buffer
	buffer[offset] = '\0';
	offset++;
	buffer = REALLOC(buffer, char, offset);
	return String(buffer);
}
void Array_::push_(Object& item) {
	if (length_ == capacity_) {
		capacity_ += ARRAY_BLOCK_SIZE;
		data_ = REALLOC(data_, Object, capacity_);
	}
	//item.addRef_();
	data_[length_++] = item;
}
Object& Array_::pop_() {
	Object& res = (Object&)null;
	if (length_ > 0) {
		res = data_[--length_];
		res.delRef_();
		if (length_ < capacity_ - ARRAY_BLOCK_SIZE) {
			capacity_ -= ARRAY_BLOCK_SIZE;
			data_ = REALLOC(data_, Object, capacity_);
		}
	}
	return res;
}
Array Array_::splice_(long long index, size_t count1, size_t count2, va_list args) {
	Array arr = null;
	if (index < 0) {
		index += length_;
	}
	if (index < (long long)length_) {
		Object* p = &data_[index];
		// remove old entries
		arr = Array(count1, p);
		for (size_t i = 0; i < count1; i++) {
			arr[i].delRef_();
		}
		size_t diff = count2 - count1;
		if (diff > 0) {
			// make room for new entries
			if (length_ + diff > capacity_) {
				capacity_ += ARRAY_BLOCK_SIZE;
				data_ = REALLOC(data_, Object, capacity_);
			}
			for (size_t i = length_ - 1; i >= index + count1; i--) {
				data_[i + diff] = data_[i];
			}
		}
		// add new entries
		for (size_t i = 0; i < count2; i++) {
			data_[i + index] = va_arg(args, Object);
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
				data_ = REALLOC(data_, Object, capacity_);
			}
		}
	}
	return arr;
}
size_t Array_::unshift_(size_t count, va_list args) {
	if (count > 0) {
		if (length_ + count > capacity_) {
			capacity_ += ARRAY_BLOCK_SIZE;
			data_ = REALLOC(data_, Object, capacity_);
		}
		// make room for new entries
		for (size_t i = length_ - 1; i >= 0; i--) {
			data_[i + count] = data_[i];
		}
		// add new entries
		for (size_t i = 0; i < count; i++) {
			data_[i] = va_arg(args, Object);
			length_++;
		}
	}
	return length_;
}


#define THIS ((Array_&)*ptr_)
#define PTR(o) ((Array_&)*(o.ptr_))
/*****************************************************************************
* Object (reference)
*****************************************************************************/
Array::Array() : Object(NULL) {
	ptr_ = NEW_(Array_);
}
Array::Array(Array& ref) : Object(NULL) {
	ptr_ = ref.ptr_;
	addRef_();
}
Array::Array(const Null& ref) : Object(ref) {
}
//Array::Array(size_t count, Object* items) {
//	ptr_ = new Array_(count, items);
//}
Array::Array(size_t count, ...) : Object(NULL) {
	va_list args;
	va_start(args, count);
	ptr_ = NEW_(Array_, count, (Object**)args);
}

Array::~Array() {
}

Array Array::concat(Array& ref) {
	Array arr;
	for (size_t i = 0; i < THIS.length_; i++) {
		arr.push(THIS.data_[i]);
	}
	for (size_t i = 0; i < PTR(ref).length_; i++) {
		arr.push(PTR(ref).data_[i]);
	}
	return arr;
}
Array Array::operator+(Array& ref) {
	return concat(ref);
}
Object& Array::operator[](long long index) {
	Object& item = (Object&)null;
	if (index < 0) {
		index += THIS.length_;
	}
	if (index < (long long)THIS.length_) {
		item = THIS.data_[index];
	}

	return item;
}
Array& Array::fill(Object& obj, size_t start, size_t end) {
	if (start < THIS.length_) {
		if (end == 0) {
			end = THIS.length_;
		}
		for (size_t i = start; i < end; i++) {
			THIS.data_[i] = obj;
		}
	}
	return *this;
}
Array Array::filter(Function& callback, Object& that) {
	Array arr;
	for (size_t i = 0; i < THIS.length_; i++) {
		// bool callback(currentValue, index, arr)
		if (*(bool*)(callback(4, that, THIS.data_[i], i, *this).valueOf()) == true) {
			arr.push(THIS.data_[i]);
		}
	}
	return arr;
}
Object Array::find(Function& callback, Object& that) {
	Object res;
	for (size_t i = 0; i < THIS.length_; i++) {
		// bool callback(currentValue, index, arr)
		if (*(bool*)(callback(4, that, THIS.data_[i], i, *this).valueOf()) == true) {
			res = THIS.data_[i];
			break;
		}
	}
	return res;
}
long long Array::findIndex(Function& callback, Object& that) {
	size_t res = -1;
	for (size_t i = 0; i < THIS.length_; i++) {
		// bool callback(currentValue, index, arr)
		if (*(bool*)(callback(4, that, THIS.data_[i], i, *this).valueOf()) == true) {
			res = i;
			break;
		}
	}
	return res;
}
void Array::forEach(Function& callback, Object& that) {
	for (size_t i = 0; i < THIS.length_; i++) {
		// bool callback(currentValue, index, arr)
		callback(4, that, THIS.data_[i], i, *this);
	}
}
long long Array::indexOf(Object& obj, size_t start) {
	long long res = -1;
	if (start < 0) {
		start += THIS.length_;
	}
	for (size_t i = start; i < THIS.length_; i++) {
		if (THIS.data_[i].compareTo(obj) == 0) {
			res = i;
			break;
		}
	}
	return res;
}
String Array::join(String& sep) {
	String str = THIS.join_(sep);
	return str;
}
long long Array::lastIndexOf(Object& obj, size_t start) {
	long long res = -1;
	if (start < 0) {
		start += THIS.length_;
	}
	for (long long i = start; i >= 0; i--) {
		if (THIS.data_[i].compareTo(obj) == 0) {
			res = i;
			break;
		}
	}
	return res;
}
Array Array::map(Function& callback, Object& that) {
	Array arr;
	for (size_t i = 0; i < THIS.length_; i++) {
		// bool callback(currentValue, index, arr)
		arr.push(callback(4, that, THIS.data_[i], i, *this));
	}
	return arr;
}
Object Array::pop() {
	return THIS.pop_();
}
void Array::push(Object& item) {
	THIS.push_(item);
}
void Array::push(size_t count, ...) {
	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; i++) {
		THIS.push_(va_arg(args, Object));
	}
}
Array& Array::reverse() {
	return *this;
}
Object Array::shift() {
	Object res;
	size_t len = THIS.length_;
	if (len > 0) {
		res = THIS.data_[0];
		for (size_t i = 1; i < len; i++) {
			THIS.data_[i - 1] = THIS.data_[i];
		}
		THIS.length_ = len - 1;
	}
	return res;
}
Array Array::slice(long long start, long long end) {
	Array arr;
	if (start < 0) start += THIS.length_;
	if (end < 0) end += THIS.length_;
	if (start > end) {
		long long tmp = start;
		start = end;
		end = tmp;
	}
	if (start < (long long)THIS.length_ && end < (long long)THIS.length_) {
		arr = Array(end - start, &THIS.data_[start]);
	}
	return arr;
}
Array& Array::sort(Function& callback) {
	return *this;
}
Array Array::splice(long long index, size_t count1, size_t count2, ...) {
	va_list args;
	va_start(args, count2);
	return THIS.splice_(index, count1, count2, args);
}
size_t Array::unshift(Object& item) {
	va_list args = (char*)&item;
	return THIS.unshift_(1, args);
}
size_t Array::unshift(size_t count, ...) {
	va_list args;
	va_start(args, count);
	return THIS.unshift_(count, args);
}

#undef THIS
#undef PTR