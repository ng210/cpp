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
	void sort_(int min, int max, COLLECTION_COMPARE* compare);
public:
	Array();
	Array(UINT32 itemSize, UINT32 capacity = ARRAY_DEFAULT_CAPACITY);
	//virtual ~Array();

	void init(UINT32 itemSize, UINT32 capacity);
	void clear();

	// Collection
	void* get(Key key);
	void* insert(Key key, void* item = NULL);
	void remove(Key key);
	void set(Key key, void* item);
	void set(void** pOldItem, void* newItem);

	int apply(COLLECTION_ACTION*, ...);
	void fill(void* value);
	void* search(Key key, Key& found, COLLECTION_COMPARE* compare = NULL);

	// ArrayBase
	int join(ArrayBase* array);
	char* str_join(const char* filler);
	Array* splice(Key pos, int count);
	Array* map(COLLECTION_ACTION* action, int itemSize);
};

NS_FW_BASE_END

#endif