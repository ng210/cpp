#ifndef __ARRAY_H
#define __ARRAY_H

#include "basedef.h"

typedef int (Callback)(int argc, ...);

NS_FW_BASE_BEGIN

template<class T> class TArray {
	//typedef int (CompareCallback)(T left, T right);
protected:
	T* data_;
	unsigned int capacity_;
	unsigned int extendSize_;
	//void sort_(CompareCallback* compare, int min, int max);
	//PROP_R(CompareCallback*, compare);
	//CompareCallback compare_;
public:
	TArray(unsigned int capacity = ARRAY_DEFAULT_CAPACITY);
	virtual ~TArray();
	void init(unsigned int capacity, T* items = NULL);
	int add(T item);
	int insertAt(int ix, T item);
	void removeAt(int ix);
	T getAt(int ix);
	T setAt(int ix, T item);

	//int join(TArray* array);
	//int forEach(Callback* action, void* args = NULL);
	//int findIndex(T key, CompareCallback* compare = TArray::compare_);
	//void sort(CompareCallback* compare = TArray::compare_);
	//int binSearch(T key, CompareCallback* compare = TArray::compare_);	// CollectionCallback* compare, int& ix, void* key);
	//int search(T key, CompareCallback* compare = TArray::compare_);		// CollectionCallback* compare, int& ix, void* key);
};

NS_FW_BASE_END

#endif