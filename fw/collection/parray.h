#ifndef __PARRAY_H
#define __PARRAY_H

#include "collection/arraybase.h"

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
	virtual void sort_(int min, int max, COLLECTION_COMPARE* compare);
public:
	PArray(UINT32 capacity = ARRAY_DEFAULT_CAPACITY);
	//virtual ~PArray();
	void init(UINT32 capacity);
	void clear();

	// Collection
	void* add(Key key, void* item = NULL);
	void* get(Key key);
	void* insert(Key key, void* item = NULL);
	void remove(Key key);
	void set(Key key, void* item);
	void set(void** pOldItem, void* newItem);

	int apply(COLLECTION_ACTION*, ...);
	void fill(void* value);
	Collection* map(COLLECTION_ACTION action, bool removeNull, ...);
	Collection* map(COLLECTION_ACTION* action);
	void* search(Key key, Key& ix, COLLECTION_COMPARE* compare = NULL);

	// ArrayBase
	int join(ArrayBase* array);
	
	char* str_join(const char* filler);
	PArray* splice(Key pos, int count);
	
	static PArray* str_split(const char* src, const char* str);
};

NS_FW_BASE_END

#endif