#ifndef __COLLECTION_TEST_H
#define __COLLECTION_TEST_H

#include "test.h"
#include "collection/collection.h"

NS_FW_BASE_USE

typedef struct TEST_DATA_ {
	int id;
	char name[16];
} TEST_DATA;

class CollectionTest : public Test {
public:
	void testArray();
	void testPArray();
	void testMap();
	void testTree();

	static int compareWord(void* item, unsigned int ix, Collection* array, void* key);
	static int compareStr(void* str, unsigned int ix, Collection* array, void* key);
	static int compareObj(void* obj, unsigned int ix, Collection* array, void* key);

	static int checkSortInt(void* item, unsigned int ix, Collection* array, void* key);
	static int checkSortStr(void* str, unsigned int ix, Collection* array, void* key);
	static int checkSortObj(void* obj, unsigned int ix, Collection* array, void* key);
};

#endif