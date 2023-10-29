#ifndef __ARRAYBASE_H
#define __ARRAYBASE_H

#include "collection/collection.h"

NS_FW_BASE_BEGIN

class ArrayBase : public Collection {
protected: PROP_R(void**, data);
protected: PROP_R(int, itemSize);
protected:
	virtual void sort_(int min, int max, COLLECTION_COMPARE* compare) = 0;
	virtual void* binSearch_(Key key, Key& found, COLLECTION_COMPARE* compare);
public:
	ArrayBase();
	virtual ~ArrayBase();

	void* binSearch(Key key, Key& found, COLLECTION_COMPARE* compare = NULL);
	Key findIndex(Key key, COLLECTION_COMPARE* compare = NULL);
	void sort(COLLECTION_COMPARE* compare = NULL);

	virtual void clear() = 0;

	virtual int join(ArrayBase* array) = 0;
	virtual char* str_join(const char* filler) = 0;

	virtual void* add(Key key, void* item = NULL);
	virtual void* push(void*);
	virtual void* pop();
};

#define ARRAY_FOREACH(array, ACTION) for (int i = 0; i < (array)->length(); i++) { \
	void* value = (array)->get(i);	\
	ACTION; \
}

NS_FW_BASE_END

#endif
