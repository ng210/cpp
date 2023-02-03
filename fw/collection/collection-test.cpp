#include "collection/collection-test.h"
#include "base/string.h"
#include "base/memory.h"
#include "base/debug.h"
#include "collection/array.h"
#include "collection/parray.h"
#include "collection/map.h"
#include "collection/graph.h"
#include "collection/tree.h"

NS_FW_BASE_USE

int CollectionTest::compareInt(void* item, Key key, unsigned int ix, Collection* array, void* args) {
	WORD i = *(int*)item;	// ((KeyValuePair*)item)->key();
	return i - key.i;
}
int CollectionTest::compareWord(void* item, Key key, unsigned int ix, Collection* array, void* args) {
	WORD w = *(WORD*)item;	// ((KeyValuePair*)item)->key();
	return w - *(WORD*)key.p;
}
int CollectionTest::compareStr(void* item, Key key, unsigned int ix, Collection* array, void* args) {
	return strncmp((char*)item, (char*)key.p);
}
int CollectionTest::compareObj(void* item, Key key, unsigned int ix, Collection* array, void* args) {
	return strncmp(((TEST_DATA*)item)->name, ((TEST_DATA*)key.p)->name);
}
int CollectionTest::checkSortInt(void* item, Key key, unsigned int ix, Collection* array, void* args) {
	int res = 1;
	if ((int)ix < array->length() - 1) {
		void* right = array->get(ix + 1);
		res = compareWord(item, right, ix, array, NULL) < 0;
	}
	return res;
}
int CollectionTest::checkSortStr(void* item, Key key, unsigned int ix, Collection* array, void* args) {
	int res = 1;
	if ((int)ix < array->length() - 1) {
		void* right = array->get(ix + 1);
		res = compareStr(item, right, ix, array, NULL) < 0;
	}
	return res;
}
int CollectionTest::checkSortObj(void* item, Key key, unsigned int ix, Collection* array, void* args) {
	int res = 1;
	if ((int)ix < array->length() - 1) {
		TEST_DATA* right = (TEST_DATA*)array->get(ix + 1);
		res = compareObj(item, right, ix, array, NULL) < 0;
	}
	return res;
}

void CollectionTest::testArray() {
	char* str = NULL;
	TEST_DATA testData = {};

	LOG(" *  Array tests\n");
	INFO(" PUSH and GET\n");
	Array* arr1 = NEW_(Array, sizeof(WORD), 4);
	Array* arr2 = NEW_(Array, 8*sizeof(char), 4);
	Array* arr3 = NEW_(Array, sizeof(TEST_DATA), 4);
	for (int i = 0; i < 16; i++) {
		arr1->push(&i);

		arr2->push("");
		str_format_s((char*)arr2->get(i), 4, "a%02d", i);

		testData.id = i;
		str_format_s((char*)&testData.name, 4, "a%02d", i);
		arr3->push(&testData);
	}
	INFO(" arr1 array<short int>\n");
	assert("length should be 16", arr1->length() == 16);
	assert("first item (#0) should be 0", *(WORD*)arr1->get(0) == 0);
	assert("item #8 should be 8", *(WORD*)arr1->get(8) == 8);
	assert("last item (#15) should be 15", *(WORD*)arr1->get(-1) == 15);

	INFO(" arr2 array<char*>\n");
	assert("length should be 16", arr2->length() == 16);
	assert("first item (#0) should be 'a00'", strncmp((char*)arr2->get(0), "a00") == 0);
	assert("item #8 should be 'a09'", strncmp((char*)arr2->get(8), "a08") == 0);
	assert("last item (#15) should be 'a15'", strncmp((char*)arr2->get(-1), "a15") == 0);

	INFO(" arr3 array<TEST_DATA>\n");
	assert("length should be 16", arr3->length() == 16);
	assert("first item (#0) should be 'a00'", strncmp(((TEST_DATA*)arr3->get(0))->name, "a00") == 0);
	assert("item #8 should be 'a09'", strncmp(((TEST_DATA*)arr3->get(8))->name, "a08") == 0);
	assert("last item (#15) should be 'a15'", strncmp(((TEST_DATA*)arr3->get(-1))->name, "a15") == 0);

	INFO(" INSERT\n");
	int dw = 100; arr1->insert(0, &dw);
	dw = 108; arr1->insert(8, &dw);
	dw = 118; arr1->insert(arr1->length(), &dw);

	arr2->insert(0, "i00");
	arr2->insert(8, "i08");
	arr2->insert(arr2->length(), "i18");

	testData.id = 0; strncpy(testData.name, 4, "i00"); arr3->insert(0, &testData);
	testData.id = 8; strncpy(testData.name, 4, "i08"); arr3->insert(8, &testData);
	testData.id = 18; strncpy(testData.name, 4, "i18"); arr3->insert(arr3->length(), &testData);

	INFO(" arr1 array<short int>\n");
	assert("length should be 19", arr1->length() == 19);
	assert("first item (#0) should be 100", *(WORD*)arr1->get(0) == 100);
	assert("item #8 should be 108", *(WORD*)arr1->get(8) == 108);
	assert("last item (#18) should be 118", *(WORD*)arr1->get(-1) == 118);

	INFO(" arr2 array<char*>\n");
	assert("length should be 19", arr2->length() == 19);
	assert("first item (#0) should be 'i00'", strncmp((char*)arr2->get(0), "i00") == 0);
	assert("item #8 should be 'i08'", strncmp((char*)arr2->get(8), "i08") == 0);
	assert("last item (#18) should be 'i18'", strncmp((char*)arr2->get(-1), "i18") == 0);

	INFO(" arr3 array<TEST_DATA>\n");
	assert("length should be 19", arr3->length() == 19);
	assert("first item (#0) should be 'i00'", strncmp(((TEST_DATA*)arr3->get(0))->name, "i00") == 0);
	assert("item #8 should be 'i08'", strncmp(((TEST_DATA*)arr3->get(8))->name, "i08") == 0);
	assert("last item (#18) should be 'i18'", strncmp(((TEST_DATA*)arr3->get(-1))->name, "i18") == 0);

	INFO(" SET\n");
	INFO(" arr1 array<short int>\n");
	dw = 508;  arr1->set(8, &dw);
	assert("item #8 should be 508", *(WORD*)arr1->get(8) == 508);

	INFO(" arr2 array<char*>\n");
	arr2->set(8, "s08");
	assert("item #8 should be 's08'", strncmp((char*)arr2->get(8), "s08") == 0);

	INFO(" arr3 array<TEST_DATA>\n");
	strncpy(testData.name, 4, "s08"); arr3->set(8, &testData);
	assert("item #8 should be 's08'", strncmp(((TEST_DATA*)arr3->get(8))->name, "s08") == 0);

	INFO(" FINDINDEX\n");
	INFO(" arr1 array<short int>\n");
	assert("item 508 should be #8", arr1->findIndex(&dw, compareWord) == 8);
	INFO(" arr2 array<char*>\n");
	assert("item 's08' should be #8", arr2->findIndex("s08", compareStr) == 8);
	INFO(" arr3 array<TEST_DATA>\n");
	assert("item 's08' should be #8", arr3->findIndex(&testData, compareObj) == 8);

	INFO(" JOIN\n");
	Array* arr12 = NEW_(Array, sizeof(WORD), 5);
	Array* arr22 = NEW_(Array, 8*sizeof(char), 5);
	Array* arr32 = NEW_(Array, sizeof(TEST_DATA), 5);
	for (int i = 0; i < 16; i++) {
		dw = i + 200;
		arr12->push(&dw);

		arr22->push("");
		str_format_s((char*)arr22->get(i), 4, "j%02d", i);

		testData.id = i;
		str_format_s((char*)&testData.name, 4, "j%02d", i);
		arr32->push(&testData);

	}
	arr1->join(arr12);
	arr2->join(arr22);
	arr3->join(arr32);

	INFO(" arr1 array<short int>\n");
	assert("array length should be 35", arr1->length() == 35);
	assert("first item (#0) should be 100", *(WORD*)arr1->get(0) == 100);
	assert("item #19 should be 200", *(WORD*)arr1->get(19) == 200);
	assert("last item (#34) should be 215", *(WORD*)arr1->get(-1) == 215);
	DEL_(arr12);

	INFO(" arr2 array<char*>\n");
	assert("array length should be 35", arr2->length() == 35);
	assert("first item (#0) should be 'i00'", strncmp((char*)arr2->get(0), "i00") == 0);
	assert("item #19 should be 'j00'", strncmp((char*)arr2->get(19), "j00") == 0);
	assert("last item (#34) should be 'j15'", strncmp((char*)arr2->get(-1), "j15") == 0);
	DEL_(arr22);

	INFO(" arr3 array<TEST_DATA>\n");
	assert("array length should be 35", arr3->length() == 35);
	assert("first item (#0) should be 'i00'", strncmp(((TEST_DATA*)arr3->get(0))->name, "i00") == 0);
	assert("item #19 should be 'j00'", strncmp(((TEST_DATA*)arr3->get(19))->name, "j00") == 0);
	assert("last item (#34) should be 'j15'", strncmp(((TEST_DATA*)arr3->get(-1))->name, "j15") == 0);
	DEL_(arr32);

	INFO(" SORT and FOREACH\n");
	INFO(" arr1 array<short int>\n");
	arr1->sort(compareWord);
	assert("sort should work in natural order", arr1->apply(checkSortInt) == -1);
	INFO(" arr2 array<char*>\n");
	arr2->sort(compareStr);
	assert("sort should work alphabetically", arr2->apply(checkSortStr) == -1);
	INFO(" arr3 array<TEST_DATA>\n");
	arr3->sort(compareObj);
	assert("sort should work alphabetically", arr3->apply(checkSortObj) == -1);

	arr2->compare(compareStr);

	INFO(" SEARCH\n");
	int pos = 0;
	INFO(" arr1 array<short int>\n");
	arr1->search(arr1->get(8), pos, compareWord);
	assert("item 508 should be #8", pos == 8);
	INFO(" arr2 array<char*>\n");
	arr2->search(arr2->get(8), pos);
	assert("item 's08' should be #8", pos == 8);
	INFO(" arr3 array<TEST_DATA>\n");
	arr3->search(arr3->get(8), pos, compareObj);
	assert("item 's08' should be #8", pos == 8);

	INFO(" BINSEARCH\n");
	INFO(" arr1 array<short int>\n");
	arr1->binSearch(arr1->get(8), pos, compareWord);
	assert("item 508 should be #8", pos == 8);
	INFO(" arr2 array<char*>\n");
	arr2->binSearch(arr2->get(8), pos);
	assert("item 's08' should be #8", pos == 8);
	INFO(" arr3 array<TEST_DATA>\n");
	arr3->binSearch(arr3->get(8), pos, compareObj);
	assert("item 's08' should be #8", pos == 8);

	INFO(" REMOVE\n");
	//assert("TODO", false);
	DEL_(arr2);
	DEL_(arr1);
	DEL_(arr3);

	INFO(" MISC.\n");
	arr1 = NEW_(Array, 8 * sizeof(char));
	arr1->push("red");
	arr1->push("blue");
	arr1->push("green");
	arr1->push("black");
	arr1->push("white");
	str = arr1->str_join(", ");
	assert("str_join should return 'red, blue, green, black, white'", strncmp(str, "red, blue, green, black, white") == 0);
	FREE(str);
	//ARRAY_FOREACH(arr1, FREE(*(char**)value));
	DEL_(arr1);
}
void CollectionTest::testPArray() {
	char* str = NULL;

	INFO(" ADD and GET\n");
	PArray* arr1 = NEW_(PArray, 4);
	for (int i = 0; i < 16; i++) {
		str = str_format("a%02d", i);
		arr1->push(str);
	}
	INFO(" arr1 array<char*>\n");
	assert("length should be 16", arr1->length() == 16);
	assert("first item (#0) should be 'a00'", strncmp((char*)arr1->get(0), "a00") == 0);
	assert("item #8 should be 'a09'", strncmp((char*)arr1->get(8), "a08") == 0);
	assert("last item (#15) should be 'a15'", strncmp((char*)arr1->get(-1), "a15") == 0);

	INFO(" INSERT\n");
	str = MALLOC(char, 4); strncpy(str, 4, "i00"); arr1->insert(0, str);
	str = MALLOC(char, 4); strncpy(str, 4, "i08"); arr1->insert(8, str);
	str = MALLOC(char, 4); strncpy(str, 4, "i18"); arr1->insert(arr1->length(), str);

	INFO(" arr1 array<char*>\n");
	assert("length should be 19", arr1->length() == 19);
	assert("first item (#0) should be 'i00'", strncmp((char*)arr1->get(0), "i00") == 0);
	assert("item #8 should be 'i08'", strncmp((char*)arr1->get(8), "i08") == 0);
	assert("last item (#18) should be 'i18'", strncmp((char*)arr1->get(-1), "i18") == 0);

	INFO(" SET\n");
	INFO(" arr1 array<char*>\n");
	FREE((char*)arr1->get(8));
	str = MALLOC(char, 4); strncpy(str, 4, "s08"); arr1->set(8, str);
	assert("item #8 should be 's08'", strncmp((char*)arr1->get(8), "s08") == 0);

	INFO(" FINDINDEX\n");
	INFO(" arr1 array<char*>\n");
	assert("item 's08' should be #8", arr1->findIndex(str, compareStr) == 8);

	INFO(" JOIN\n");
	PArray* arr12 = NEW_(PArray, 5);
	for (int i = 0; i < 16; i++) {
		str = str_format("j%02d", i);
		arr1->push(str);
	}
	arr1->join(arr12);

	INFO(" arr1 array<char*>\n");
	assert("array length should be 35", arr1->length() == 35);
	assert("first item (#0) should be 'i00'", strncmp((char*)arr1->get(0), "i00") == 0);
	assert("item #19 should be 'j00'", strncmp((char*)arr1->get(19), "j00") == 0);
	assert("last item (#34) should be 'j15'", strncmp((char*)arr1->get(-1), "j15") == 0);
	DEL_(arr12);

	INFO(" SORT and FOREACH\n");
	INFO(" arr1 array<char*>\n");
	arr1->sort(compareStr);
	assert("sort should work alphabetically", arr1->apply(checkSortStr) == -1);

	arr1->compare(compareStr);

	INFO(" SEARCH\n");
	int pos = 0;
	INFO(" arr1 array<char*>\n");
	arr1->search(arr1->get(8), pos);
	assert("item 's08' should be #8", pos == 8);

	INFO(" BINSEARCH\n");
	INFO(" arr1 array<char*>\n");
	arr1->binSearch(arr1->get(8), pos);
	assert("item 's08' should be #8", pos == 8);

	INFO(" REMOVE\n");
	while (arr1->length() > 0) {
		FREE((char*)arr1->get(-1));
		arr1->remove(-1);
	}
	assert("arr1 should be empty", arr1->length() == 0);
	DEL_(arr1);

	INFO(" MISC.\n");
	PArray* pArr1 = PArray::str_split("  red,blue.  green-black white", ",. -");
	int errors = 0;
	if (strncmp((char*)pArr1->get(0), "red")) errors++;
	if (strncmp((char*)pArr1->get(1), "blue")) errors++;
	if (strncmp((char*)pArr1->get(2), "green")) errors++;
	if (strncmp((char*)pArr1->get(3), "black")) errors++;
	if (strncmp((char*)pArr1->get(4), "white")) errors++;
	assert("str_split should return array with 5 items", errors == 0);
	str = pArr1->str_join(", ");
	assert("str_join should return 'red, blue, green, black, white'", strncmp(str, "red, blue, green, black, white") == 0);
	FREE(str);
	ARRAY_FOREACH(pArr1, FREE(value));
	DEL_(pArr1);
}
void CollectionTest::testMap() {
	char* str = NULL;
	WORD dw = 0;
	TEST_DATA testData;

	Map* map1 = NEW_(Map, sizeof(WORD), sizeof(WORD), Map::hashingInt, compareInt); map1->hasRefKey(false);
	Map* map2 = NEW_(Map, MAP_USE_REF, MAP_USE_REF, Map::hashingStr, compareStr);
	Map* map3 = NEW_(Map, sizeof(TEST_DATA), sizeof(TEST_DATA), Map::hashingItem, compareObj);

	LOG(" *  Map tests\n");
	INFO(" ADD and GET\n");
	for (int i = 0; i < 16; i++) {
		str = str_format("a%02d", i);
		testData.id = i;
		strncpy(testData.name, 4, str);
		map1->put(i, &i);
		map2->put(str, str);
		map3->put(&testData, &testData);
	}
	INFO(" map1 map<int, int>\n");
	assert("length should be 16", map1->size() == 16);
	dw = 0; assert("first item (#0) should be 0", *(word*)map1->get(dw) == 0);
	dw = 8; assert("item #8 should be 8", *(word*)map1->get(dw) == 8);
	dw = 15; assert("last item (#15) should be 15", *(word*)map1->get(dw) == 15);
	dw = -1; assert("-1 should return NULL", map1->get(dw) == NULL);

	INFO(" map2 map<char*, char*>\n");
	assert("length should be 16", map2->size() == 16);
	str = "a00"; assert("first item (#0) should be 'a00'", strncmp((char*)map2->get(str), "a00") == 0);
	str = "a08"; assert("item #8 should be 'a09'", strncmp((char*)map2->get(str), "a08") == 0);
	str = "a15"; assert("last item (#15) should be 'a15'", strncmp((char*)map2->get(str), "a15") == 0);
	str = "a16"; assert("'a16' should return NULL", map2->get(str) == NULL);

	INFO(" map3 map<TEST_DATA, TEST_DATA>\n");
	assert("length should be 16", map3->size() == 16);
	strncpy(testData.name, 4, "a00"); testData.id = 0; assert("first item (#0) should be 'a00'", strncmp(((TEST_DATA*)map3->get(&testData))->name, "a00") == 0);
	strncpy(testData.name, 4, "a08"); testData.id = 8; assert("item #8 should be 'a09'", strncmp(((TEST_DATA*)map3->get(&testData))->name, "a08") == 0);
	strncpy(testData.name, 4, "a15"); testData.id = 15; assert("last item (#15) should be 'a15'", strncmp(((TEST_DATA*)map3->get(&testData))->name, "a15") == 0);
	strncpy(testData.name, 4, "a16"); testData.id = 16; assert("'a16' should return NULL", map3->get(&testData) == NULL);

	DEL_(map1);
	MAP_FOREACH(map2, FREE((char*)key););
	DEL_(map2);
	DEL_(map3);
}

void CollectionTest::testTree() {
	Tree* tree1 = NEW_(Tree);
	INFO(" Build a complete 3-level binary tree\n");
	// Build a complete 3-level binary tree
	int nodeCount = 1, nix = 1;
	for (int l = 0; l < 3; l++) {
		for (int n = 0; n < nodeCount; n++) {
			TEST_DATA* testData = MALLOC(TEST_DATA, 1);
			str_format_s(testData->name, 16, "n%03d", nix);
			testData->id = nix;
			if (l > 0) {
				int ix = (nodeCount>>1) - 1 + (n >> 1);
				Vertex* parent = (Vertex*)tree1->vertices().get(ix);
				int id = ((TEST_DATA*)parent->data())->id;
				char* edgeData = MALLOC(char, 16);
				str_format_s(edgeData, 16, "e%03d->%03d", id, nix);
				INFO(" %d->%d", id, nix);
				Vertex* node = tree1->addVertex(parent, testData, edgeData);
				//edgeValue = MALLOC(char, 16);
				//sprintf_s(edgeValue, 16, "e%04d<-%04d", nix, id);
				//tree1->addEdge(node, parent, edgeValue);
			} else {
				INFO(" 0->%d", nix);
				Vertex* node = tree1->addVertex(NULL, testData, NULL);
			}
			nix++;
		}
		nodeCount <<= 1;
	}
	INFO("\n");
	Vertex* node = tree1->root();
	node = ((Edge*)node->edges().get(0))->to();
	assert("node 0/0 should have id 'n002'", NS_FW_BASE::strncmp(((TEST_DATA*)node->data())->name, "n002", 4) == 0);
	node = ((Edge*)node->edges().get(1))->to();
	assert("node 0/0/1 should have id 'n005'", NS_FW_BASE::strncmp(((TEST_DATA*)node->data())->name, "n005", 4) == 0);
	node = tree1->root();
	node = ((Edge*)node->edges().get(1))->to();
	assert("node 0/1 should have id 'n003'", NS_FW_BASE::strncmp(((TEST_DATA*)node->data())->name, "n003", 4) == 0);
	node = ((Edge*)node->edges().get(0))->to();
	assert("node 0/1/0 should have id 'n006'", NS_FW_BASE::strncmp(((TEST_DATA*)node->data())->name, "n006", 4) == 0);

	int path1[] = { 0, 0, 0, -1 };
	assert("node on path 0/0/0 should be 'n004'", NS_FW_BASE::strncmp(((TEST_DATA*)tree1->get(path1)->data())->name, "n004", 4) == 0);
	path1[1] = 1;
	assert("node on path 0/1/0 should be 'n006'", NS_FW_BASE::strncmp(((TEST_DATA*)tree1->get(path1)->data())->name, "n006", 4) == 0);
	path1[2] = 1;
	assert("node on path 0/1/1 should be 'n007'", NS_FW_BASE::strncmp(((TEST_DATA*)tree1->get(path1)->data())->name, "n007", 4) == 0);

	int path2[] = { 2, 0, -1 };
	assert("node on path 2/0 should be 'n006'", NS_FW_BASE::strncmp(((TEST_DATA*)tree1->get(path2)->data())->name, "n006", 4) == 0);
	path2[1] = 3;
	assert("node on path 2/3 should be NULL", tree1->get(path2) == NULL);

	//ARRAY_FOREACH(tree1->edges(), delEdge((Edge*)value););
	//ARRAY_FOREACH(tree1->nodes(), delNode((Node*)value););
	DEL_(tree1);
}

void CollectionTest::runAll(int& totalPassed, int& totalFailed) {
	test("Array tests", (TestMethod)&CollectionTest::testArray);
	test("PArray tests", (TestMethod)&CollectionTest::testPArray);
	test("Map tests", (TestMethod)&CollectionTest::testMap);
	test("Tree tests", (TestMethod)&CollectionTest::testTree);
	totalPassed += totalPassed_;
	totalFailed += totalFailed_;
}