#ifndef __COLLECTION_TEST_H
#define __COLLECTION_TEST_H

#include "test.h"
#include "collection/collection.h"

NS_FW_BASE_USE

typedef struct TEST_DATA_ {
	int id;
	char name[12];
} TEST_DATA;

class CollectionTest : public Test {
public:
	void testArray();
	void testPArray();
	void testMap();
	void testTree();

	void runAll(int& totalPassed, int& totalFailed);

	static COLLECTION_COMPARE compareInt;
	static COLLECTION_COMPARE compareWord;
	static COLLECTION_COMPARE compareStr;
	static COLLECTION_COMPARE compareObj;

	static COLLECTION_ACTION checkSortInt;
	static COLLECTION_ACTION checkSortStr;
	static COLLECTION_ACTION checkSortObj;
};

#endif