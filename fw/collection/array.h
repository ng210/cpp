#ifndef __ARRAY_H
#define __ARRAY_H

#include "collection/arraybase.h"

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
	void sort_(int min, int max, COLLECTIONCALLBACK* compare);
public:
	Array();
	Array(UINT32 itemSize, UINT32 capacity = ARRAY_DEFAULT_CAPACITY);
	//virtual ~Array();

	void init(UINT32 itemSize, UINT32 capacity, void* items = NULL);
	void clear();

	// Collection
	void* get(Key key);
	void* insert(Key key, void* item = NULL);
	void remove(Key key);
	void set(Key key, void* item);
	void set(void** pOldItem, void* newItem);

	int apply(COLLECTIONCALLBACK*, ...);
	void fill(void* value);
	void* search(Key key, int& ix, COLLECTIONCALLBACK* compare = NULL);

	// ArrayBase
	int join(ArrayBase* array);
	char* str_join(const char* filler);
};

NS_FW_BASE_END

#endif