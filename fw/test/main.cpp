#include "base/consoleapp.h"

NS_FW_BASE_USE

int testUtils();
int testStr();
int testArray();
int testMap();
int testBuffer();
int testFile();
int testPathInfo();

#define TEST(t) t() ? failed++ : passed++;

int _main(Map* args) {
//int main(int argc, char* argv[]) {
	LOG("Framework tests\n\nCommand line arguments:\n");

	for (UINT32 i = 0; i < args->keys()->length(); i++) {
		char* key = (char*)args->keys()->getAt(i);
		LOG(" - %s : %s\n", key, (char*)args->get(key));
	}
	LOG("\n");

	//Debug::set(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
	int passed = 0, failed = 0;

	//TEST(testUtils);
	//TEST(testStr);
	//TEST(testArray);
	//TEST(testMap);
	//TEST(testBuffer);
	//TEST(testFile);
	TEST(testPathInfo);

	LOG("****************\n\nFinal results: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));

	return 0;
}

typedef struct TEST_DATA_ {
	int id;
	char name[4];
} TEST_DATA;

#define ASSERT(label, expr) printf(" - " label ": "); if (expr) { passed++; printf("passed\n"); } else { failed++; printf("failed\n"); };

int compareInt(void* item, unsigned int ix, Collection* array, void* key) {
	WORD dw = *(WORD*)item;	// ((KeyValuePair*)item)->key();
	return dw - *(WORD*)key;
}
int compareStr(void* item, unsigned int ix, Collection* array, void* key) {
	return strncmp(*(char**)item, *(char**)key);
}
int compareObj(void* item, unsigned int ix, Collection* array, void* key) {
	return strncmp(((TEST_DATA*)item)->name, ((TEST_DATA*)key)->name);
}
int checkSortInt(void* item, unsigned int ix, Collection* array, void* arg) {
	int res = 1;
	if ((int)ix < array->length() - 1) {
		void* right = array->getAt(ix + 1);
		res = compareInt(item, ix, array, right) < 0;
	}
	return res;
}
int checkSortStr(void* item, unsigned int ix, Collection* array, void* arg) {
	int res = 1;
	if ((int)ix < array->length() - 1) {
		void* right = array->getAt(ix + 1);
		res = compareStr(item, ix, array, right) < 0;
	}
	return res;
}
int checkSortObj(void* item, unsigned int ix, Collection* array, void* arg) {
	int res = 1;
	if ((int)ix < array->length() - 1) {
		TEST_DATA* right = (TEST_DATA*)array->getAt(ix + 1);
		res = compareObj(item, ix, array, right) < 0;
	}
	return res;
}

int testUtils() {
	int passed = 0;
	int failed = 0;
	unsigned char* src = MALLOC(unsigned char, 679);
	for (int i = 0; i < 679; i++) {
		src[i] = i & 0xff;
	}
	unsigned char* dst = MALLOC(unsigned char, 1127);
	memcpy(&dst[13], src, 679);
	int errors = 0;
	for (int i = 0; i < 679; i++) {
		if (dst[13 + i] != (i & 0xff)) {
			errors++;
		}
	}
	ASSERT("memcpy should work correctly", errors == 0);

	FREE(src);
	FREE(dst);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testBuffer() {
	int passed = 0;
	int failed = 0;

	Buffer* buf1 = NEW_(Buffer);
	UINT32 number = 0;
	for (UINT32 i = 0; i < 20; i++) {
		UINT32 buffer[50000];
		for (UINT32 j = 0; j < 50000; j++) {
			buffer[j] = number++;
		}
		buf1->append(buffer, sizeof(UINT32) * 50000);
	}
	int errors = 0;
	number = 0;
	for (UINT32 i = 0; i < 20 * 50000; i++) {
		UINT32 uint32;
		buf1->readAsType<UINT32>(&uint32, 1, i, 0);
		if (uint32 != number) errors++;
		number++;
	}
	ASSERT("buffer write/read should work correctly", errors == 0);
	if (errors != 0) {
		LOG(" errors: %d\n", errors);
	}
	UINT32* buffer = buf1->getBufferAsType<UINT32>();
	errors = 0;
	number = 0;
	for (UINT32 i = 0; i < 20 * 50000; i++) {
		if (buffer[i] != number) errors++;
		number++;
	}
	ASSERT("getBuffer should return the whole buffer as type UINT32", errors == 0);
	FREE(buffer);
	DEL_(buf1);
	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testFile() {
	int passed = 0;
	int failed = 0;

	char* buffer = "Hello world! This is a test for the File static class.";
	File::write("test.dat", (BYTE*)buffer, NS_FW_BASE::strlen(buffer));
	BYTE* buf1 = File::read("test.dat");
	ASSERT("buffer should be written to file", NS_FW_BASE::strncmp(buffer, (const char*)buf1) == 0);
	FREE(buf1);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testPathInfo() {
	int passed = 0;
	int failed = 0;
	const char* fullPath = "C:\\code\\git\\cpp\\fw\\test\\main.cpp";
	const char* path = "C:\\code\\git\\cpp\\fw\\test";
	const char* file = "main";
	const char* extension = "cpp";
	PathInfo pathInfo(fullPath);
	ASSERT("path should be correct", NS_FW_BASE::strncmp(pathInfo.path(), path) == 0);
	ASSERT("path should be correct", NS_FW_BASE::strncmp(pathInfo.fileName(), file) == 0);
	ASSERT("path should be correct", NS_FW_BASE::strncmp(pathInfo.extension(), extension) == 0);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testStr() {
	int passed = 0;
	int failed = 0;
	const char* src = "  Hello world!  ";
	ASSERT("strspn(' Helo') should return 8", strspn(src, " Helo") == 8);
	ASSERT("strcspn('lo') should return 4", strcspn(src, "lo") == 4);
	ASSERT("strrspn(' orld!') should return 9", strrspn(src, " rld!") == 9);
	ASSERT("strrcspn('Hello') should return 11", strrcspn(src, "Hello") == 11);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testArray() {
	int passed = 0;
	int failed = 0;
	char* str = NULL;
	TEST_DATA testData;

	LOG("********************************\nArray tests\n********\n");
	LOG(" ADD and GET\n");
	Array* arr1 = NEW_(Array, sizeof(WORD), 4);
	Array* arr2 = NEW_(Array, sizeof(char*), 4);
	Array* arr3 = NEW_(Array, sizeof(TEST_DATA), 4);
	for (int i = 0; i < 16; i++) {
		arr1->add(&i);

		str = MALLOC(char, 4);
		sprintf_s(str, 4, "a%02d", i);
		arr2->add(&str);

		testData.id = i;
		strncpy(testData.name, 4, str);
		arr3->add(&testData);
	}
	LOG(" arr1 array<short int>\n");
	ASSERT("length should be 16", arr1->length() == 16);
	ASSERT("first item (#0) should be 0", *(WORD*)arr1->getAt(0) == 0);
	ASSERT("item #8 should be 8", *(WORD*)arr1->getAt(8) == 8);
	ASSERT("last item (#15) should be 15", *(WORD*)arr1->getAt(-1) == 15);

	LOG(" arr2 array<char*>\n");
	ASSERT("length should be 16", arr2->length() == 16);
	ASSERT("first item (#0) should be 'a00'", strncmp(*(char**)arr2->getAt(0), "a00") == 0);
	ASSERT("item #8 should be 'a09'", strncmp(*(char**)arr2->getAt(8), "a08") == 0);
	ASSERT("last item (#15) should be 'a15'", strncmp(*(char**)arr2->getAt(-1), "a15") == 0);

	LOG(" arr3 array<TEST_DATA>\n");
	ASSERT("length should be 16", arr3->length() == 16);
	ASSERT("first item (#0) should be 'a00'", strncmp(((TEST_DATA*)arr3->getAt(0))->name, "a00") == 0);
	ASSERT("item #8 should be 'a09'", strncmp(((TEST_DATA*)arr3->getAt(8))->name, "a08") == 0);
	ASSERT("last item (#15) should be 'a15'", strncmp(((TEST_DATA*)arr3->getAt(-1))->name, "a15") == 0);

	LOG(" INSERT\n");
	WORD dw = 100; arr1->insertAt(0, &dw);
	dw = 108; arr1->insertAt(8, &dw);
	dw = 118; arr1->insertAt(arr1->length(), &dw);

	str = MALLOC(char, 4); strncpy(str, 4, "i00"); arr2->insertAt(0, &str);
	str = MALLOC(char, 4); strncpy(str, 4, "i08"); arr2->insertAt(8, &str);
	str = MALLOC(char, 4); strncpy(str, 4, "i18"); arr2->insertAt(arr2->length(), &str);

	testData.id = 0; strncpy(testData.name, 4, "i00"); arr3->insertAt(0, &testData);
	testData.id = 8; strncpy(testData.name, 4, "i08"); arr3->insertAt(8, &testData);
	testData.id = 18; strncpy(testData.name, 4, "i18"); arr3->insertAt(arr3->length(), &testData);

	LOG(" arr1 array<short int>\n");
	ASSERT("length should be 19", arr1->length() == 19);
	ASSERT("first item (#0) should be 100", *(WORD*)arr1->getAt(0) == 100);
	ASSERT("item #8 should be 108", *(WORD*)arr1->getAt(8) == 108);
	ASSERT("last item (#18) should be 118", *(WORD*)arr1->getAt(-1) == 118);

	LOG(" arr2 array<char*>\n");
	ASSERT("length should be 19", arr2->length() == 19);
	ASSERT("first item (#0) should be 'i00'", strncmp(*(char**)arr2->getAt(0), "i00") == 0);
	ASSERT("item #8 should be 'i08'", strncmp(*(char**)arr2->getAt(8), "i08") == 0);
	ASSERT("last item (#18) should be 'i18'", strncmp(*(char**)arr2->getAt(-1), "i18") == 0);

	LOG(" arr3 array<TEST_DATA>\n");
	ASSERT("length should be 19", arr3->length() == 19);
	ASSERT("first item (#0) should be 'i00'", strncmp(((TEST_DATA*)arr3->getAt(0))->name, "i00") == 0);
	ASSERT("item #8 should be 'i08'", strncmp(((TEST_DATA*)arr3->getAt(8))->name, "i08") == 0);
	ASSERT("last item (#18) should be 'i18'", strncmp(((TEST_DATA*)arr3->getAt(-1))->name, "i18") == 0);

	LOG(" SET\n");
	LOG(" arr1 array<short int>\n");
	dw = 508;  arr1->setAt(8, &dw);
	ASSERT("item #8 should be 208", *(WORD*)arr1->getAt(8) == 508);

	LOG(" arr2 array<char*>\n");
	FREE(*(char**)arr2->getAt(8));
	str = MALLOC(char, 4); strncpy(str, 4, "s08"); arr2->setAt(8, &str);
	ASSERT("item #8 should be 's08'", strncmp(*(char**)arr2->getAt(8), "s08") == 0);

	LOG(" arr3 array<TEST_DATA>\n");
	strncpy(testData.name, 4, "s08"); arr3->setAt(8, &testData);
	ASSERT("item #8 should be 's08'", strncmp(((TEST_DATA*)arr3->getAt(8))->name, "s08") == 0);

	LOG(" FINDINDEX\n");
	LOG(" arr1 array<short int>\n");
	ASSERT("item 508 should be #8", arr1->findIndex(&dw, compareInt) == 8);
	LOG(" arr2 array<char*>\n");
	ASSERT("item 's08' should be #8", arr2->findIndex(&str, compareStr) == 8);
	LOG(" arr3 array<TEST_DATA>\n");
	ASSERT("item 's08' should be #8", arr3->findIndex(&testData, compareObj) == 8);

	LOG(" JOIN\n");
	Array* arr12 = NEW_(Array, sizeof(WORD), 5);
	Array* arr22 = NEW_(Array, sizeof(char*), 5);
	Array* arr32 = NEW_(Array, sizeof(TEST_DATA), 5);
	for (int i = 0; i < 16; i++) {
		dw = i + 200;
		arr1->add(&dw);

		str = MALLOC(char, 4);
		sprintf_s(str, 4, "j%02d", i);
		arr2->add(&str);

		testData.id = i;
		strncpy(testData.name, 4, str);
		arr3->add(&testData);

	}
	arr1->join(arr12);
	arr2->join(arr22);
	arr3->join(arr32);

	LOG(" arr1 array<short int>\n");
	ASSERT("array length should be 35", arr1->length() == 35);
	ASSERT("first item (#0) should be 100", *(WORD*)arr1->getAt(0) == 100);
	ASSERT("item #19 should be 200", *(WORD*)arr1->getAt(19) == 200);
	ASSERT("last item (#34) should be 215", *(WORD*)arr1->getAt(-1) == 215);
	DEL_(arr12);

	LOG(" arr2 array<char*>\n");
	ASSERT("array length should be 35", arr2->length() == 35);
	ASSERT("first item (#0) should be 'i00'", strncmp(*(char**)arr2->getAt(0), "i00") == 0);
	ASSERT("item #19 should be 'j00'", strncmp(*(char**)arr2->getAt(19), "j00") == 0);
	ASSERT("last item (#34) should be 'j15'", strncmp(*(char**)arr2->getAt(-1), "j15") == 0);
	DEL_(arr22);

	LOG(" arr3 array<TEST_DATA>\n");
	ASSERT("array length should be 35", arr3->length() == 35);
	ASSERT("first item (#0) should be 'i00'", strncmp(((TEST_DATA*)arr3->getAt(0))->name, "i00") == 0);
	ASSERT("item #19 should be 'j00'", strncmp(((TEST_DATA*)arr3->getAt(19))->name, "j00") == 0);
	ASSERT("last item (#34) should be 'j15'", strncmp(((TEST_DATA*)arr3->getAt(-1))->name, "j15") == 0);
	DEL_(arr32);

	LOG(" SORT and FOREACH\n");
	LOG(" arr1 array<short int>\n");
	arr1->sort(compareInt);
	ASSERT("sort should work in natural order", arr1->forEach(checkSortInt) == -1);
	LOG(" arr2 array<char*>\n");
	arr2->sort(compareStr);
	ASSERT("sort should work alphabetically", arr2->forEach(checkSortStr) == -1);
	LOG(" arr3 array<TEST_DATA>\n");
	arr3->sort(compareObj);
	ASSERT("sort should work alphabetically", arr3->forEach(checkSortObj) == -1);

	arr2->compare(compareStr);

	LOG(" SEARCH\n");
	int pos = 0;
	LOG(" arr1 array<short int>\n");
	arr1->search(arr1->getAt(8), pos, compareInt);
	ASSERT("item 508 should be #8", pos = 8);
	LOG(" arr2 array<char*>\n");
	arr2->search(arr2->getAt(8), pos);
	ASSERT("item 's08' should be #8", pos = 8);
	LOG(" arr3 array<TEST_DATA>\n");
	arr3->search(arr3->getAt(8), pos, compareObj);
	ASSERT("item 's08' should be #8", pos = 8);

	LOG(" BINSEARCH\n");
	LOG(" arr1 array<short int>\n");
	arr1->binSearch(arr1->getAt(8), pos, compareInt);
	ASSERT("item 508 should be #8", pos = 8);
	LOG(" arr2 array<char*>\n");
	arr2->binSearch(arr2->getAt(8), pos);
	ASSERT("item 's08' should be #8", pos = 8);
	LOG(" arr3 array<TEST_DATA>\n");
	arr3->binSearch(arr3->getAt(8), pos, compareObj);
	ASSERT("item 's08' should be #8", pos = 8);

	LOG(" REMOVE\n");
	while (arr2->length() > 0) {
		FREE(*(char**)arr2->getAt(-1));
		arr2->removeAt(-1);
	}
	ASSERT("array2 should be empty", arr2->length() == 0);
	DEL_(arr2);
	DEL_(arr1);
	DEL_(arr3);

	LOG(" MISC.\n");
	arr1 = Array::strsplit("  red,blue.  green-black white", ",. -");
	int errors = 0;
	if (strncmp(*(char**)arr1->getAt(0), "red")) errors++;
	if (strncmp(*(char**)arr1->getAt(1), "blue")) errors++;
	if (strncmp(*(char**)arr1->getAt(2), "green")) errors++;
	if (strncmp(*(char**)arr1->getAt(3), "black")) errors++;
	if (strncmp(*(char**)arr1->getAt(4), "white")) errors++;
	ASSERT("strsplit should return array with 5 items", errors == 0);
	ARRAY_FOREACH(arr1, FREE(*(char**)value));
	DEL_(arr1);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testMap() {
	int passed = 0;
	int failed = 0;
	char* str = NULL;
	word dw = 0;
	TEST_DATA testData;

	Map* map1 = NEW_(Map, sizeof(int), sizeof(int), compareInt);
	Map* map2 = NEW_(Map, sizeof(char*), sizeof(char*), compareStr);
	Map* map3 = NEW_(Map, sizeof(TEST_DATA), sizeof(TEST_DATA), compareObj);

	LOG("********************************\nMap tests\n********\n");
	LOG(" ADD and GET\n");
	for (int i = 0; i < 16; i++) {
		str = MALLOC(char, 4);
		sprintf_s(str, 4, "a%02d", i);
		testData.id = i;
		strncpy(testData.name, 4, str);
		map1->put(&i, &i);
		map2->put(&str, &str);
		map3->put(&testData, &testData);
	}
	LOG(" map1 map<int, int>\n");
	ASSERT("length should be 16", map1->size() == 16);
	dw = 0;  ASSERT("first item (#0) should be 0", *(WORD*)map1->get(&dw) == 0);
	dw = 8; ASSERT("item #8 should be 8", *(WORD*)map1->get(&dw) == 8);
	dw = 15; ASSERT("last item (#15) should be 15", *(WORD*)map1->get(&dw) == 15);
	dw = -1; ASSERT("-1 should return NULL", map1->get(&dw) == NULL);

	LOG(" map2 map<char*, char*>\n");
	ASSERT("length should be 16", map2->size() == 16);
	str = "a00"; ASSERT("first item (#0) should be 'a00'", strncmp(*(char**)map2->get(&str), "a00") == 0);
	str = "a08"; ASSERT("item #8 should be 'a09'", strncmp(*(char**)map2->get(&str), "a08") == 0);
	str = "a15"; ASSERT("last item (#15) should be 'a15'", strncmp(*(char**)map2->get(&str), "a15") == 0);
	str = "a16"; ASSERT("'a16' should return NULL", map2->get(&str) == NULL);

	LOG(" map3 map<TEST_DATA, TEST_DATA>\n");
	ASSERT("length should be 16", map3->size() == 16);
	strncpy(testData.name, 4, "a00"); ASSERT("first item (#0) should be 'a00'", strncmp(((TEST_DATA*)map3->get(&testData))->name, "a00") == 0);
	strncpy(testData.name, 4, "a08"); ASSERT("item #8 should be 'a09'", strncmp(((TEST_DATA*)map3->get(&testData))->name, "a08") == 0);
	strncpy(testData.name, 4, "a15"); ASSERT("last item (#15) should be 'a15'", strncmp(((TEST_DATA*)map3->get(&testData))->name, "a15") == 0);
	strncpy(testData.name, 4, "a16"); ASSERT("'a16' should return NULL", map3->get(&testData) == NULL);

	DEL_(map1);
	MAP_FOREACH(map2, FREE(*(char**)key));
	DEL_(map2);
	DEL_(map3);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testTree() {
	int passed = 0;
	int failed = 0;


	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;

}