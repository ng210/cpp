//#include "base/consoleapp.h"
#include <windows.h>
#include "runtime.h"
#include "utils/directory.h"
#include "consoleapp.h"

NS_FW_BASE_USE

int testUtils();
int testBuffer();
int testFile();
int testDirectory();
int testPathInfo();
int testStr();
int testString();
int testArray();
int testPArray();
int testMap();
int testTree();

#define TEST(t) t() ? failed++ : passed++;

int _main(NS_FW_BASE::Map* args) {
//int main(int argc, char* args[]) {
	#ifdef _DEBUG
	Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
	#endif
	LOG("Framework tests\n\nCommand line arguments:\n");

	//for (UINT32 i = 0; i < args->keys()->length(); i++) {
	//	char* key = *(char**)args->keys()->getAt(i);
	//	LOG(" - %s : %s\n", key, *(char**)args->get(key));
	//}
	//LOG("\n");

	int passed = 0, failed = 0;

	//TEST(testUtils);
	//TEST(testBuffer);
	TEST(testFile);
	//TEST(testDirectory);
	//TEST(testPathInfo);
	//TEST(testStr);
	//TEST(testString);
	//TEST(testArray);
	//TEST(testPArray);
	//TEST(testMap);
	//TEST(testTree);
	
	//WORD buffer[48000];
	//for (int i = 0; i < 48000; i++) {
	//	buffer[i] = (WORD)Utils::random(65536);
	//}
	//File::write("random.smp", (BYTE*)&buffer, sizeof(WORD) * 48000);

	//for (int i = 0; i < 16; i++) {
	//	printf("%d. %s\n", i, (char*)arr.getAt(i));
	//}

	printf("****************\n\nFinal results: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));

	return 0;
}

typedef struct TEST_DATA_ {
	int id;
	char name[16];
} TEST_DATA;

#define ASSERT(label, expr) LOG(" - " label ": "); if (expr) { passed++; LOG("passed\n"); } else { failed++; LOG("failed\n"); };

int compareWord(void* item, unsigned int ix, Collection* array, void* key) {
	WORD w = *(WORD*)item;	// ((KeyValuePair*)item)->key();
	return w - *(WORD*)key;
}
int compareStr(void* item, unsigned int ix, Collection* array, void* key) {
	return strncmp((char*)item, (char*)key);
}
int compareObj(void* item, unsigned int ix, Collection* array, void* key) {
	return strncmp(((TEST_DATA*)item)->name, ((TEST_DATA*)key)->name);
}
int checkSortInt(void* item, unsigned int ix, Collection* array, void* arg) {
	int res = 1;
	if ((int)ix < array->length() - 1) {
		void* right = array->getAt(ix + 1);
		res = compareWord(item, ix, array, right) < 0;
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

void delNode(Node* node) {
	TEST_DATA* ptr = (TEST_DATA*)node->value();
	FREE(ptr);
	DEL_(node);
}
void delEdge(Edge* edge) {
	FREE(edge->value());
	DEL_(edge);
}

int testUtils() {
	int passed = 0;
	int failed = 0;
	LOG("********************************\nUtils tests\n********\n");

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
	LOG("********************************\nBuffer tests\n********\n");

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
	LOG("********************************\nFile tests\n********\n");

	char* data = "Hello world! This is a test for the File static class.";
	char* data2 = &data[10];
	File::write("test.dat", (BYTE*)data, NS_FW_BASE::strlen(data));
	BYTE* bytes = NULL;
	size_t bytesRead = File::read("test.dat", &bytes);
	ASSERT("byte buffer should be written into an read from file", NS_FW_BASE::strncmp(data, (const char*)bytes, bytesRead) == 0);
	FREE(bytes); bytes = NULL;
	bytesRead = File::read("test.dat", &bytes, 10, 0);
	ASSERT("file from an offset should be read into byte buffer", NS_FW_BASE::strncmp(data2, (const char*)bytes, bytesRead) == 0);
	FREE(bytes); bytes = NULL;
	bytesRead = File::read("test.dat", &bytes, 10, 10);
	ASSERT("10 bytes were read from file", bytesRead == 10);
	ASSERT("chunk of file should be read into byte buffer", NS_FW_BASE::strncmp(data2, (const char*)bytes, 10) == 0);
	FREE(bytes); bytes = NULL;

	Buffer* buffer = NEW_(Buffer);
	buffer->append(data, fmw::strlen(data));
	File::write("test2.dat", buffer);
	buffer->clear();
	bytesRead = File::read("test2.dat", buffer);
	bytes = buffer->getByteBuffer();
	ASSERT("buffer contents should be written into an read from file", NS_FW_BASE::strncmp(data, (const char*)bytes, bytesRead) == 0);
	FREE(bytes);
	DEL_(buffer);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testDirectory() {
	int passed = 0;
	int failed = 0;
	LOG("********************************\nDirectory tests\n********\n");

	char* files[] = {
		//"1asu.xm",
		//"test.dat",
		//"ENGLISH.PAK",
		"chapter7.pak"
	};
	char* basePath = str_substr(getWorkingDir(), 0, str_indexOf(getWorkingDir(), "test") + 4);
	char* path = str_concat(basePath, "\\data");
	FREE(basePath);

	Array* entries = Directory::getFiles(path, true, ".pak");
	ARRAY_FOREACH(entries, printf("%s\n", (char*)value));
	int notFound = arraysize(files);
	for (int i = 0; i < arraysize(files); i++) {
		int ix = 0;
		if (entries->findIndex(files[i], Collection::compareStr) != -1) {
			LOG("-%s\n", files[i]);
			notFound--;
		}
	}
	notFound == 0 ? passed++ : failed++;

	DEL_(entries);
	FREE(path);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testPathInfo() {
	int passed = 0;
	int failed = 0;
	LOG("********************************\nPathInfo tests\n********\n");

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
	LOG("********************************\nStr tests\n********\n");
	const char* src = "  Hello world!  ";
	ASSERT("strspn(' Helo') should return 8", strspn(src, " Helo") == 8);
	ASSERT("strcspn('lo') should return 4", strcspn(src, "lo") == 4);
	ASSERT("strrspn(' rld!') should return 10", strrspn(src, " rld!") == 10);
	ASSERT("strrcspn('Hello') should return 11", strrcspn(src, "Hello") == 11);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testString() {
	int passed = 0;
	int failed = 0;
	LOG("********************************\nString tests\n********\n");

	char* str = NULL;
	str = str_concat("Hello", "world");
	ASSERT("concat should create 'Helloworld", strncmp(str, "Helloworld") == 0);
	FREE(str);
	str = str_concat(4, "Concat test: ", "Hello", " ", "world!");
	ASSERT("concat should create 'Concat test: Hello world!'", strncmp(str, "Concat test: Hello world!") == 0);
	FREE(str);
	ASSERT("string should end with 'world!'", str_ends("Hello world!", "world!") != 0);
	ASSERT("string should not end with 'qorld!'", str_ends("Hello world!", "qorld!") == 0);
	ASSERT("string should start with 'Hello'", str_starts("Hello world!", "Hello") != 0);
	ASSERT("string should not start with 'Bello'", str_starts("Hello world!", "Bello") == 0);
	const char* term1 = "The red fox jumps over a red fence.";
	ASSERT("indexOf 'red' in 'The red fox jumps over a red fence.' should be 4", str_indexOf(term1, "red") == 4);
	ASSERT("indexOf 'red' in 'The red fox jumps over a red fence.' starting at 8 should be 25", str_indexOf(term1, "red", 8) == 25);
	ASSERT("indexOf 'red' in 'The red fox jumps over a red fence.' starting at 26 should be -1", str_indexOf(term1, "Hello", 2) == -1);
	ASSERT("lastIndexOf 'red' in 'The red fox jumps over a red fence.' should be 25", str_lastIndexOf(term1, "red") == 25);
	ASSERT("lastIndexOf 'blue' in 'The red fox jumps over a red fence.' should be -1", str_lastIndexOf(term1, "blue") == -1);
	str = str_join(4, " - ", "Join", "test", "Hello", "world!");
	ASSERT("join should return 'Join - test - Hello - world!", strncmp(str, "Join - test - Hello - world!") == 0);
	FREE(str);
	str = str_replace(term1, "red", "blue");
	ASSERT("replace should return 'The blue fox jumps over a blue fence.'", strncmp(str, "The blue fox jumps over a blue fence.") == 0);
	FREE(str);
	str = str_replace(term1, "red", "x");
	ASSERT("replace should return 'The x fox jumps over a x fence.'", strncmp(str, "The x fox jumps over a x fence.") == 0);
	FREE(str);
	str = str_replace(term1, "red ", "");
	ASSERT("replace should return 'The fox jumps over a fence.'", strncmp(str, "The fox jumps over a fence.") == 0);
	FREE(str);
	str = str_substring(term1, 12);
	ASSERT("substr(12) should return 'jumps over a red fence.'", strncmp(str, "jumps over a red fence.") == 0);
	FREE(str);
	str = str_substring(term1, 12, 17);
	ASSERT("substr(12, 17) should return 'jumps'", strncmp(str, "jumps") == 0);
	FREE(str);
	str = str_substring(term1, 12, -7);
	ASSERT("substr(12, -7) should return 'jumps over a red'", strncmp(str, "jumps over a red") == 0);
	FREE(str);
	str = str_substring(term1, -17, -18);
	ASSERT("substr(-17, -18) should return 'The red fox jumps'", strncmp(str, "The red fox jumps") == 0);
	FREE(str);
	str = str_substring(term1, 4, 17);
	ASSERT("substr(4, 17) should return 'red fox jumps'", strncmp(str, "red fox jumps") == 0);
	FREE(str);
	str = str_substring(term1, 17, 4);
	ASSERT("substr(17, 4) should return 'red fox jumps'", strncmp(str, "red fox jumps") == 0);
	FREE(str);
	str = str_substring(term1, -107, -111);
	ASSERT("substr(-107, -111) should return ''", strncmp(str, "") == 0);
	FREE(str);
	char** arr = str_split(term1, "red");
	ASSERT("split should return at #0 'The '", strncmp(arr[0], "The ") == 0);
	ASSERT("split should return at #1 ' fox jumps over a '", strncmp(arr[1], " fox jumps over a ") == 0);
	ASSERT("split should return at #2 ' fence.'", strncmp(arr[2], " fence.") == 0);
	ASSERT("split should return at #3 NULL", arr[3] == NULL);
	str = str_join(arr, "red");
	ASSERT("join of split results should return the original string", strncmp(str, term1) == 0);
	FREE(str);
	for (int i = 0; (str = arr[i]) != NULL; i++) {
		FREE(str);
	}
	FREE(arr);
	arr = str_split(" Hello world! ", " ");
	ASSERT("split should return at #0 'Hello'", strncmp(arr[0], "Hello") == 0);
	ASSERT("split should return at #1 'world!'", strncmp(arr[1], "world!") == 0);
	for (int i = 0; (str = arr[i]) != NULL; i++) {
		FREE(str);
	}
	FREE(arr);

	const char* term2 = " \t Hello \t world!\n \t ";
	str = str_ltrim(term2);
	ASSERT("ltrim should return 'Hello \\t world!\\n \\t ", strncmp(str, "Hello \t world!\n \t ") == 0);
	FREE(str);
	str = str_rtrim(term2);
	ASSERT("rtrim should return ' \\t Hello \\t world!", strncmp(str, " \t Hello \t world!") == 0);
	FREE(str);
	str = str_trim(term2);
	ASSERT("trim should return 'Hello \\t world!", strncmp(str, "Hello \t world!") == 0);
	FREE(str);

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
	Array* arr2 = NEW_(Array, 8*sizeof(char), 4);
	Array* arr3 = NEW_(Array, sizeof(TEST_DATA), 4);
	for (int i = 0; i < 16; i++) {
		arr1->add(&i);

		arr2->add("");
		sprintf_s((char*)arr2->getAt(i), 4, "a%02d", i);

		testData.id = i;
		sprintf_s((char*)&testData.name, 4, "a%02d", i);
		arr3->add(&testData);
	}
	LOG(" arr1 array<short int>\n");
	ASSERT("length should be 16", arr1->length() == 16);
	ASSERT("first item (#0) should be 0", *(WORD*)arr1->getAt(0) == 0);
	ASSERT("item #8 should be 8", *(WORD*)arr1->getAt(8) == 8);
	ASSERT("last item (#15) should be 15", *(WORD*)arr1->getAt(-1) == 15);

	LOG(" arr2 array<char*>\n");
	ASSERT("length should be 16", arr2->length() == 16);
	ASSERT("first item (#0) should be 'a00'", strncmp((char*)arr2->getAt(0), "a00") == 0);
	ASSERT("item #8 should be 'a09'", strncmp((char*)arr2->getAt(8), "a08") == 0);
	ASSERT("last item (#15) should be 'a15'", strncmp((char*)arr2->getAt(-1), "a15") == 0);

	LOG(" arr3 array<TEST_DATA>\n");
	ASSERT("length should be 16", arr3->length() == 16);
	ASSERT("first item (#0) should be 'a00'", strncmp(((TEST_DATA*)arr3->getAt(0))->name, "a00") == 0);
	ASSERT("item #8 should be 'a09'", strncmp(((TEST_DATA*)arr3->getAt(8))->name, "a08") == 0);
	ASSERT("last item (#15) should be 'a15'", strncmp(((TEST_DATA*)arr3->getAt(-1))->name, "a15") == 0);

	LOG(" INSERT\n");
	int dw = 100; arr1->insertAt(0, &dw);
	dw = 108; arr1->insertAt(8, &dw);
	dw = 118; arr1->insertAt(arr1->length(), &dw);

	arr2->insertAt(0, "i00");
	arr2->insertAt(8, "i08");
	arr2->insertAt(arr2->length(), "i18");

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
	ASSERT("first item (#0) should be 'i00'", strncmp((char*)arr2->getAt(0), "i00") == 0);
	ASSERT("item #8 should be 'i08'", strncmp((char*)arr2->getAt(8), "i08") == 0);
	ASSERT("last item (#18) should be 'i18'", strncmp((char*)arr2->getAt(-1), "i18") == 0);

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
	arr2->setAt(8, "s08");
	ASSERT("item #8 should be 's08'", strncmp((char*)arr2->getAt(8), "s08") == 0);

	LOG(" arr3 array<TEST_DATA>\n");
	strncpy(testData.name, 4, "s08"); arr3->setAt(8, &testData);
	ASSERT("item #8 should be 's08'", strncmp(((TEST_DATA*)arr3->getAt(8))->name, "s08") == 0);

	LOG(" FINDINDEX\n");
	LOG(" arr1 array<short int>\n");
	ASSERT("item 508 should be #8", arr1->findIndex(&dw, compareWord) == 8);
	LOG(" arr2 array<char*>\n");
	ASSERT("item 's08' should be #8", arr2->findIndex("s08", compareStr) == 8);
	LOG(" arr3 array<TEST_DATA>\n");
	ASSERT("item 's08' should be #8", arr3->findIndex(&testData, compareObj) == 8);

	LOG(" JOIN\n");
	Array* arr12 = NEW_(Array, sizeof(WORD), 5);
	Array* arr22 = NEW_(Array, 8*sizeof(char), 5);
	Array* arr32 = NEW_(Array, sizeof(TEST_DATA), 5);
	for (int i = 0; i < 16; i++) {
		dw = i + 200;
		arr12->add(&dw);

		arr22->add("");
		sprintf_s((char*)arr22->getAt(i), 4, "j%02d", i);

		testData.id = i;
		sprintf_s((char*)&testData.name, 4, "j%02d", i);
		arr32->add(&testData);

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
	ASSERT("first item (#0) should be 'i00'", strncmp((char*)arr2->getAt(0), "i00") == 0);
	ASSERT("item #19 should be 'j00'", strncmp((char*)arr2->getAt(19), "j00") == 0);
	ASSERT("last item (#34) should be 'j15'", strncmp((char*)arr2->getAt(-1), "j15") == 0);
	DEL_(arr22);

	LOG(" arr3 array<TEST_DATA>\n");
	ASSERT("array length should be 35", arr3->length() == 35);
	ASSERT("first item (#0) should be 'i00'", strncmp(((TEST_DATA*)arr3->getAt(0))->name, "i00") == 0);
	ASSERT("item #19 should be 'j00'", strncmp(((TEST_DATA*)arr3->getAt(19))->name, "j00") == 0);
	ASSERT("last item (#34) should be 'j15'", strncmp(((TEST_DATA*)arr3->getAt(-1))->name, "j15") == 0);
	DEL_(arr32);

	LOG(" SORT and FOREACH\n");
	LOG(" arr1 array<short int>\n");
	arr1->sort(compareWord);
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
	arr1->search(arr1->getAt(8), pos, compareWord);
	ASSERT("item 508 should be #8", pos = 8);
	LOG(" arr2 array<char*>\n");
	arr2->search(arr2->getAt(8), pos);
	ASSERT("item 's08' should be #8", pos = 8);
	LOG(" arr3 array<TEST_DATA>\n");
	arr3->search(arr3->getAt(8), pos, compareObj);
	ASSERT("item 's08' should be #8", pos = 8);

	LOG(" BINSEARCH\n");
	LOG(" arr1 array<short int>\n");
	arr1->binSearch(arr1->getAt(8), pos, compareWord);
	ASSERT("item 508 should be #8", pos = 8);
	LOG(" arr2 array<char*>\n");
	arr2->binSearch(arr2->getAt(8), pos);
	ASSERT("item 's08' should be #8", pos = 8);
	LOG(" arr3 array<TEST_DATA>\n");
	arr3->binSearch(arr3->getAt(8), pos, compareObj);
	ASSERT("item 's08' should be #8", pos = 8);

	LOG(" REMOVE\n");
	//ASSERT("TODO", false);
	DEL_(arr2);
	DEL_(arr1);
	DEL_(arr3);

	LOG(" MISC.\n");
	arr1 = NEW_(Array, 8 * sizeof(char));
	arr1->add("red");
	arr1->add("blue");
	arr1->add("green");
	arr1->add("black");
	arr1->add("white");
	str = arr1->str_join(", ");
	ASSERT("str_join should return 'red, blue, green, black, white'", strncmp(str, "red, blue, green, black, white") == 0);
	FREE(str);
	//ARRAY_FOREACH(arr1, FREE(*(char**)value));
	DEL_(arr1);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testPArray() {
	int passed = 0;
	int failed = 0;
	char* str = NULL;

	LOG("********************************\nPArray tests\n********\n");
	LOG(" ADD and GET\n");
	PArray* arr1 = NEW_(PArray, 4);
	for (int i = 0; i < 16; i++) {
		str = MALLOC(char, 4);
		sprintf_s(str, 4, "a%02d", i);
		arr1->add(str);
	}
	LOG(" arr1 array<char*>\n");
	ASSERT("length should be 16", arr1->length() == 16);
	ASSERT("first item (#0) should be 'a00'", strncmp((char*)arr1->getAt(0), "a00") == 0);
	ASSERT("item #8 should be 'a09'", strncmp((char*)arr1->getAt(8), "a08") == 0);
	ASSERT("last item (#15) should be 'a15'", strncmp((char*)arr1->getAt(-1), "a15") == 0);

	LOG(" INSERT\n");
	str = MALLOC(char, 4); strncpy(str, 4, "i00"); arr1->insertAt(0, str);
	str = MALLOC(char, 4); strncpy(str, 4, "i08"); arr1->insertAt(8, str);
	str = MALLOC(char, 4); strncpy(str, 4, "i18"); arr1->insertAt(arr1->length(), str);

	LOG(" arr1 array<char*>\n");
	ASSERT("length should be 19", arr1->length() == 19);
	ASSERT("first item (#0) should be 'i00'", strncmp((char*)arr1->getAt(0), "i00") == 0);
	ASSERT("item #8 should be 'i08'", strncmp((char*)arr1->getAt(8), "i08") == 0);
	ASSERT("last item (#18) should be 'i18'", strncmp((char*)arr1->getAt(-1), "i18") == 0);

	LOG(" SET\n");
	LOG(" arr1 array<char*>\n");
	FREE((char*)arr1->getAt(8));
	str = MALLOC(char, 4); strncpy(str, 4, "s08"); arr1->setAt(8, str);
	ASSERT("item #8 should be 's08'", strncmp((char*)arr1->getAt(8), "s08") == 0);

	LOG(" FINDINDEX\n");
	LOG(" arr1 array<char*>\n");
	ASSERT("item 's08' should be #8", arr1->findIndex(str, compareStr) == 8);

	LOG(" JOIN\n");
	PArray* arr12 = NEW_(PArray, 5);
	for (int i = 0; i < 16; i++) {
		str = MALLOC(char, 4);
		sprintf_s(str, 4, "j%02d", i);
		arr1->add(str);
	}
	arr1->join(arr12);

	LOG(" arr1 array<char*>\n");
	ASSERT("array length should be 35", arr1->length() == 35);
	ASSERT("first item (#0) should be 'i00'", strncmp((char*)arr1->getAt(0), "i00") == 0);
	ASSERT("item #19 should be 'j00'", strncmp((char*)arr1->getAt(19), "j00") == 0);
	ASSERT("last item (#34) should be 'j15'", strncmp((char*)arr1->getAt(-1), "j15") == 0);
	DEL_(arr12);

	LOG(" SORT and FOREACH\n");
	LOG(" arr1 array<char*>\n");
	arr1->sort(compareStr);
	ASSERT("sort should work alphabetically", arr1->forEach(checkSortStr) == -1);

	arr1->compare(compareStr);

	LOG(" SEARCH\n");
	int pos = 0;
	LOG(" arr1 array<char*>\n");
	arr1->search(arr1->getAt(8), pos);
	ASSERT("item 's08' should be #8", pos = 8);

	LOG(" BINSEARCH\n");
	LOG(" arr1 array<char*>\n");
	arr1->binSearch(arr1->getAt(8), pos);
	ASSERT("item 's08' should be #8", pos = 8);

	LOG(" REMOVE\n");
	while (arr1->length() > 0) {
		FREE((char*)arr1->getAt(-1));
		arr1->removeAt(-1);
	}
	ASSERT("arr1 should be empty", arr1->length() == 0);
	DEL_(arr1);

	LOG(" MISC.\n");
	PArray* pArr1 = PArray::str_split("  red,blue.  green-black white", ",. -");
	int errors = 0;
	if (strncmp((char*)pArr1->getAt(0), "red")) errors++;
	if (strncmp((char*)pArr1->getAt(1), "blue")) errors++;
	if (strncmp((char*)pArr1->getAt(2), "green")) errors++;
	if (strncmp((char*)pArr1->getAt(3), "black")) errors++;
	if (strncmp((char*)pArr1->getAt(4), "white")) errors++;
	ASSERT("str_split should return array with 5 items", errors == 0);
	str = pArr1->str_join(", ");
	ASSERT("str_join should return 'red, blue, green, black, white'", strncmp(str, "red, blue, green, black, white") == 0);
	FREE(str);
	ARRAY_FOREACH(pArr1, FREE(value));
	DEL_(pArr1);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testMap() {
	int passed = 0;
	int failed = 0;
	char* str = NULL;
	WORD dw = 0;
	TEST_DATA testData;

	Map* map1 = NEW_(Map, sizeof(WORD), sizeof(WORD), Map::hashingItem, compareWord);
	Map* map2 = NEW_(Map, MAP_USE_REF, MAP_USE_REF, Map::hashingStr, compareStr);
	Map* map3 = NEW_(Map, sizeof(TEST_DATA), sizeof(TEST_DATA), Map::hashingItem, compareObj);

	LOG("********************************\nMap tests\n********\n");
	LOG(" ADD and GET\n");
	for (int i = 0; i < 16; i++) {
		str = MALLOC(char, 4);
		sprintf_s(str, 4, "a%02d", i);
		testData.id = i;
		strncpy(testData.name, 4, str);
		map1->put(&i, &i);
		map2->put(str, str);
		map3->put(&testData, &testData);
	}
	LOG(" map1 map<int, int>\n");
	ASSERT("length should be 16", map1->size() == 16);
	dw = 0; ASSERT("first item (#0) should be 0", *(WORD*)map1->get(&dw) == 0);
	dw = 8; ASSERT("item #8 should be 8", *(WORD*)map1->get(&dw) == 8);
	dw = 15; ASSERT("last item (#15) should be 15", *(WORD*)map1->get(&dw) == 15);
	dw = -1; ASSERT("-1 should return NULL", map1->get(&dw) == NULL);

	LOG(" map2 map<char*, char*>\n");
	ASSERT("length should be 16", map2->size() == 16);
	str = "a00"; ASSERT("first item (#0) should be 'a00'", strncmp((char*)map2->get(str), "a00") == 0);
	str = "a08"; ASSERT("item #8 should be 'a09'", strncmp((char*)map2->get(str), "a08") == 0);
	str = "a15"; ASSERT("last item (#15) should be 'a15'", strncmp((char*)map2->get(str), "a15") == 0);
	str = "a16"; ASSERT("'a16' should return NULL", map2->get(str) == NULL);

	LOG(" map3 map<TEST_DATA, TEST_DATA>\n");
	ASSERT("length should be 16", map3->size() == 16);
	strncpy(testData.name, 4, "a00"); testData.id = 0; ASSERT("first item (#0) should be 'a00'", strncmp(((TEST_DATA*)map3->get(&testData))->name, "a00") == 0);
	strncpy(testData.name, 4, "a08"); testData.id = 8; ASSERT("item #8 should be 'a09'", strncmp(((TEST_DATA*)map3->get(&testData))->name, "a08") == 0);
	strncpy(testData.name, 4, "a15"); testData.id = 15; ASSERT("last item (#15) should be 'a15'", strncmp(((TEST_DATA*)map3->get(&testData))->name, "a15") == 0);
	strncpy(testData.name, 4, "a16"); testData.id = 16; ASSERT("'a16' should return NULL", map3->get(&testData) == NULL);

	DEL_(map1);
	MAP_FOREACH(map2, FREE((char*)key););
	DEL_(map2);
	DEL_(map3);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;
}
int testTree() {
	int passed = 0;
	int failed = 0;

	LOG("********************************\nTree tests\n********\n");
	Tree* tree1 = NEW_(Tree);
	LOG(" Build a complete 3-level binary tree\n");
	// Build a complete 3-level binary tree
	int nodeCount = 1, nix = 1;
	for (int l = 0; l < 3; l++) {
		for (int n = 0; n < nodeCount; n++) {
			TEST_DATA* testData = MALLOC(TEST_DATA, 1);
			sprintf_s(testData->name, 16, "n%03d", nix);
			testData->id = nix;
			if (l > 0) {
				int ix = (nodeCount>>1) - 1 + (n >> 1);
				Node* parent = (Node*)tree1->nodes()->getAt(ix);
				int id = ((TEST_DATA*)parent->value())->id;
				char* edgeValue = MALLOC(char, 16);
				sprintf_s(edgeValue, 16, "e%03d->%03d", id, nix);
				LOG(" %d->%d", id, nix);
				Node* node = tree1->addNode(parent, testData, edgeValue);
				//edgeValue = MALLOC(char, 16);
				//sprintf_s(edgeValue, 16, "e%04d<-%04d", nix, id);
				//tree1->addEdge(node, parent, edgeValue);
			} else {
				LOG(" 0->%d", nix);
				Node* node = tree1->addNode(NULL, testData);
			}
			nix++;
		}
		nodeCount <<= 1;
	}
	LOG("\n");
	Node* node = tree1->root();
	node = ((Edge*)node->edges()->values()->getAt(0))->to();
	ASSERT("node 0/0 should have id 'n002'", NS_FW_BASE::strncmp(((TEST_DATA*)node->value())->name, "n002", 4) == 0);
	node = ((Edge*)node->edges()->values()->getAt(1))->to();
	ASSERT("node 0/0/1 should have id 'n005'", NS_FW_BASE::strncmp(((TEST_DATA*)node->value())->name, "n005", 4) == 0);
	node = tree1->root();
	node = ((Edge*)node->edges()->values()->getAt(1))->to();
	ASSERT("node 0/1 should have id 'n003'", NS_FW_BASE::strncmp(((TEST_DATA*)node->value())->name, "n003", 4) == 0);
	node = ((Edge*)node->edges()->values()->getAt(0))->to();
	ASSERT("node 0/1/0 should have id 'n006'", NS_FW_BASE::strncmp(((TEST_DATA*)node->value())->name, "n006", 4) == 0);

	int path1[] = { 0, 0, 0, -1 };
	ASSERT("node on path 0/0/0 should be 'n004'", NS_FW_BASE::strncmp(((TEST_DATA*)tree1->get(path1)->value())->name, "n004", 4) == 0);
	path1[1] = 1;
	ASSERT("node on path 0/1/0 should be 'n006'", NS_FW_BASE::strncmp(((TEST_DATA*)tree1->get(path1)->value())->name, "n006", 4) == 0);
	path1[2] = 1;
	ASSERT("node on path 0/1/1 should be 'n007'", NS_FW_BASE::strncmp(((TEST_DATA*)tree1->get(path1)->value())->name, "n007", 4) == 0);

	int path2[] = { 2, 0, -1 };
	ASSERT("node on path 2/0 should be 'n006'", NS_FW_BASE::strncmp(((TEST_DATA*)tree1->get(path2)->value())->name, "n006", 4) == 0);
	path2[1] = 3;
	ASSERT("node on path 2/3 should be NULL", tree1->get(path2) == NULL);

	//ARRAY_FOREACH(tree1->edges(), delEdge((Edge*)value););
	//ARRAY_FOREACH(tree1->nodes(), delNode((Node*)value););
	DEL_(tree1);

	LOG("****************\n\nResults: %d/%d=%.02f%%\n\n********************************\n\n",
		passed, (passed + failed), (100.0f*passed) / (passed + failed));
	return failed;

}