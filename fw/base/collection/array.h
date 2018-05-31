#ifndef __ARRAY_H
#define __ARRAY_H

#include "Collection/arraybase.h"

NS_FW_BASE_BEGIN

#define ARRAY_DEFAULT_CAPACITY 16

//*****************************************************************************
// Array
//*****************************************************************************
class Array : public ArrayBase {
//protected: PROP_R(char*, data);
//protected: PROP_R(UINT32, itemSize);
protected:
	UINT32 capacity_;
	UINT32 extendSize_;
	void sort_(int min, int max, CollectionCallback* compare);
	//void* binSearch_(void* key, int& ix, CollectionCallback* compare);
	//PROP(CollectionCallback*, compare);
public:
	Array(UINT32 itemSize, UINT32 capacity = ARRAY_DEFAULT_CAPACITY);
	//virtual ~Array();
	void init(UINT32 itemSize, UINT32 capacity, void* items = NULL);
	void clear();
	/* Collection */
	void* add(void* item);
	void* insertAt(int ix, void* item);
	void removeAt(int ix);
	void* getAt(int ix);
	void setAt(int ix, void* item);
	// ArrayBase
	int join(ArrayBase* array);
	int forEach(CollectionCallback* action, void* args = NULL);
	void* search(void* key, int& ix, CollectionCallback* compare = NULL);
	char* str_join(const char* filler);

	//int binSearch(void* key, int& ix, CollectionCallback* compare = NULL);
	//int findIndex(void* key, CollectionCallback* compare = NULL);
	//void sort(CollectionCallback* compare = NULL);
	
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

NS_FW_BASE_END

#endif