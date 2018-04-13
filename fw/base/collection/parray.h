#ifndef __PARRAY_H
#define __PARRAY_H

#include "Collection/arraybase.h"

NS_FW_BASE_BEGIN

#define ARRAY_DEFAULT_CAPACITY 16

//*****************************************************************************
// Array
//*****************************************************************************
class PArray : public ArrayBase {
//protected: PROP_R(void**, data);
protected:
	UINT32 capacity_;
	UINT32 extendSize_;
	virtual void sort_(int min, int max, CollectionCallback* compare);
	//virtual void* binSearch_(void* key, int& ix, CollectionCallback* compare);
	//PROP(CollectionCallback*, compare);
public:
	PArray(UINT32 capacity = ARRAY_DEFAULT_CAPACITY);
	//virtual ~PArray();
	void init(UINT32 capacity, void* items = NULL);
	// Collection
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

	static PArray* str_split(const char* src, const char* str);
};

NS_FW_BASE_END

#endif