#ifndef __ARRAYBASE_H
#define __ARRAYBASE_H

#include "collection/collection.h"

NS_FW_BASE_BEGIN

class ArrayBase : public Collection {
protected: PROP_R(void**, data);
protected: PROP_R(int, itemSize);
protected:
	virtual void sort_(int min, int max, COLLECTIONCALLBACK* compare) = 0;
	virtual void* binSearch_(Key key, int& ix, COLLECTIONCALLBACK* compare);
public:
	ArrayBase();
	virtual ~ArrayBase();

	void* binSearch(Key key, int& ix, COLLECTIONCALLBACK* compare = NULL);
	int findIndex(Key key, COLLECTIONCALLBACK* compare = NULL);
	void sort(COLLECTIONCALLBACK* compare = NULL);

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
