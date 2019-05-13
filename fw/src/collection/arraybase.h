#ifndef __ARRAYBASE_H

#include "collection/collection.h"

#define __ARRAYBASE_H

NS_FW_BASE_BEGIN

class ArrayBase : public Collection {
protected: PROP_R(void**, data);
protected: PROP_R(int, itemSize);
protected:
	virtual void sort_(int min, int max, CollectionCallback* compare) = 0;
	virtual void* binSearch_(void* key, int& ix, CollectionCallback* compare);
protected: PROP(CollectionCallback*, compare);
public:
	virtual ~ArrayBase();
	void* binSearch(void* key, int& ix, CollectionCallback* compare = NULL);
	int findIndex(void* key, CollectionCallback* compare = NULL);
	void sort(CollectionCallback* compare = NULL);

	virtual int join(ArrayBase* array) = 0;
	virtual int forEach(CollectionCallback* action, void* args = NULL) = 0;
	virtual void* search(void* key, int& ix, CollectionCallback* compare = NULL) = 0;
	virtual char* str_join(const char* filler) = 0;
};

#define ARRAY_FOREACH(array, ACTION) for (UINT32 i = 0; i < (array)->length(); i++) { \
	void* value = (array)->getAt(i);	\
	ACTION; \
}

NS_FW_BASE_END

#endif
