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

	void runAll(int& totalPassed, int& totalFailed);

	static int compareInt(void* item, Key key, unsigned int ix, Collection* array, void* args);
	static int compareWord(void* item, Key key, unsigned int ix, Collection* array, void* args);
	static int compareStr(void* str, Key key, unsigned int ix, Collection* array, void* args);
	static int compareObj(void* obj, Key key, unsigned int ix, Collection* array, void* args);

	static int checkSortInt(void* item, Key key, unsigned int ix, Collection* array, void* args);
	static int checkSortStr(void* str, Key key, unsigned int ix, Collection* array, void* args);
	static int checkSortObj(void* obj, Key key, unsigned int ix, Collection* array, void* args);
};

#endif