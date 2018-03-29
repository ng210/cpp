#ifndef __ARRAY_H
#define __ARRAY_H

#include "Collection/collection.h"

NS_FW_BASE_BEGIN

#define ARRAY_DEFAULT_CAPACITY 16

//*****************************************************************************
// Array
//*****************************************************************************
class Array : public Collection {
protected: PROP_R(char*, data);
protected: 
	UINT32 itemSize_;
	UINT32 capacity_;
	UINT32 extendSize_;
	void sort_(int min, int max, CollectionCallback* compare);
	int binSearch_(void* key, int& ix, CollectionCallback* compare);
	PROP(CollectionCallback*, compare);
public:
	Array() {};
	Array(UINT32 itemSize, UINT32 capacity = ARRAY_DEFAULT_CAPACITY);
	virtual ~Array();
	void init(UINT32 itemSize, UINT32 capacity, void* items = NULL);
	void* add(void* item);
	void* insertAt(int ix, void* item);
	void removeAt(int ix);
	void* getAt(int ix);
	void* setAt(int ix, void* item);
	int join(Array* array);
	int forEach(CollectionCallback* action, void* args = NULL);
	int findIndex(void* key, CollectionCallback* compare = NULL);
	void sort(CollectionCallback* compare = NULL);
	int binSearch(void* key, int& ix, CollectionCallback* compare = NULL);
	int search(void* key, int& ix, CollectionCallback* compare = NULL);

	static Array* strsplit(const char* src, const char* str);
};
//
//template <typename T> class Array : public Collection<T> {
//protected: T* data_;
//public:
//	Array(size_t capacity = ARRAY_DEFAULT_CAPACITY);
//	void init(size_t capacity, T* items = NULL);
//	T add(T item);
//	T insertAt(int ix, T item);
//	T removeAt(int ix);
//	T getAt(int ix);
//	int findIndex(Compare* compare);
//};

#define ARRAY_FOREACH(array, ACTION) for (UINT32 i = 0; i < array->length(); i++) { \
	void* value = array->getAt(i);	\
	ACTION; \
}

NS_FW_BASE_END

#endif