#include <stdio.h>
#include <math.h>

#include "utils/utils.h"
#include "consoleapp.h"

#define MEM_DEBUG

NS_FW_BASE_USE

#define ASSERT(label, expr) printf(" - " label " "); if (expr) { passed++; printf("passed\n"); } else { failed++; printf("failed\n"); };
#define ADD(arr, ix, type, ...) arr[ix] = NEW_(type, ##__VA_ARGS__)
#define SET(arr, dst, src, fn, ...) arr[dst] = arr[src]->##fn(##__VA_ARGS__)

void testObjectPtr() {

}

void testStringPtr() {
	int passed = 0;
	int failed = 0;

	String* arr[50];
	for (int i = 0; i < 50; i++) arr[i] = NULL;

	ADD(arr, 0, String, "AbCdEf");
	SET(arr, 1, 0, concat, "_");
	ADD(arr, 2, String, "GhIjKl");
	SET(arr, 3, 1, concat, arr[2]);
	SET(arr, 4, 3, concat, " ");
	ADD(arr, 5, String, "AbMnOpQr");
	String* str = SET(arr, 6, 4, concat, arr[5]);

	ASSERT("Concat with const char*", str->strcmp("AbCdEf_GhIjKl AbMnOpQr")==0);
	ASSERT("Character at 2", (*str)[2] == 'C');
	ASSERT("Character at -1", (*str)[-1] == 0);
	ASSERT("Character at 30", (*str)[30] == 0);

	ASSERT("Starts with 'AbCdEf'", str->startsWith("AbCdEf"));
	ASSERT("Starts with <str0>", str->startsWith(arr[0]));
	ASSERT("Does not start with <str2>", !str->startsWith(arr[2]));

	ASSERT("indexOf <str0> is 0", str->indexOf(arr[0]) == 0);
	ASSERT("indexOf <str2> is 7", str->indexOf(arr[2]) == 7);
	ASSERT("indexOf <str6> is 0", str->indexOf(arr[6]) == 0);
	ASSERT("indexOf 'Ab' is 0", str->indexOf("Ab") == 0);
	ASSERT("indexOf 'Ba' is -1", str->indexOf("Ba") == -1);
	ASSERT("indexOf 'IjKl' from 5 is 9", str->indexOf("IjKl", 5) == 9);

	str = SET(arr, 7, 6, concat, arr[0]);
	ASSERT("lastIndexOf <str0> is 22", str->lastIndexOf(arr[0]) == 22);
	ASSERT("lastIndexOf <str2> is 7", str->lastIndexOf(arr[2]) == 7);
	ASSERT("lastIndexOf <str7> is 0", str->lastIndexOf(arr[6]) == 0);
	ASSERT("lastIndexOf 'AbM' is 14", str->lastIndexOf("AbM") == 14);
	ASSERT("lastIndexOf 'Ba' is -1", str->lastIndexOf("Ba") == -1);

	ASSERT("Ends with 'AbCdEf'", str->endsWith("AbCdEf"));
	ASSERT("Ends with <str0>", str->endsWith(arr[0]));
	ASSERT("Does not end with <str2>", !str->endsWith(arr[2]));

	ADD(arr, 8, String, "A red fox jumps over Fred's red fence.");
	str = SET(arr, 9, 8, replace, "red", "small");
	ASSERT("Replace 'red' by 'small'", str->strcmp("A small fox jumps over Fsmall's small fence.")==0);
	str = SET(arr, 10, 9, replace, "red", "blue");
	ASSERT("Replace 'red' by 'blue'", str->compareTo(arr[9]) == 0);
	str = SET(arr, 11, 9, replace, "small", "");
	ASSERT("Replace 'red' by ''", str->strcmp("A  fox jumps over F's  fence.") == 0);

	ADD(arr, 12, String, "red");
	ADD(arr, 13, String, "small");
	str = SET(arr, 14, 8, replace, arr[12],arr[13]);
	ASSERT("Replace (String)'red' by (String)'small'", str->strcmp("A small fox jumps over Fsmall's small fence.") == 0);
	
	str = SET(arr, 15, 8, substring, 2, 9);
	ASSERT("Substring(2, 9)", str->strcmp("red fox") == 0);
	str = SET(arr, 16, 8, substring, -5, 9);
	ASSERT("Substring(-5, 9)", str->strcmp("A red fox") == 0);
	str = SET(arr, 17, 8, substring, -22, 8);
	ASSERT("Substring(-22, 8)", str->strcmp("A red fox") == 0);
	str = SET(arr, 18, 8, substring, 2, -7);
	ASSERT("Substring(2, -7)", str->strcmp("red fox jumps over Fred's red fence.") == 0);

	str = SET(arr, 19, 8, substr, 2, 7);
	ASSERT("Substr(2, 7)", str->strcmp("red fox") == 0);
	str = SET(arr, 20, 8, substr, -6, 9);
	ASSERT("Substr(-6, 9)", str->strcmp("fence.") == 0);
	str = SET(arr, 21, 8, substr, -22, 9);
	ASSERT("Substr(-22, 9)", str->strcmp("over Fred's red fence.") == 0);
	str = SET(arr, 22, 8, substr, 2, -7);
	ASSERT("Substr(2, -7)", str->strcmp("red fox jumps over Fred's red fence.") == 0);

	String** tokens = arr[8]->split_(" ");
	str = arr[23] = String::join(tokens, " ");
	ASSERT("Split(' ') & Join(\" \")", str->compareTo(arr[8]) == 0);
	str = arr[24] = String::join(tokens);
	ASSERT("Join()", str->strcmp("AredfoxjumpsoverFred'sredfence.") == 0);
	ADD(arr, 25, String, "##");
	str = arr[26] = String::join(tokens, arr[25]);
	ASSERT("Join(String('##'))", str->strcmp("A##red##fox##jumps##over##Fred's##red##fence.") == 0);
	size_t i = 0;
	while ((str = tokens[i]) != NULL) {
		DEL_(str);
		i++;
	}
	DEL_(tokens);

	str = SET(arr, 27, 8, toUpperCase);
	ASSERT("toUpperCase()", str->strcmp("A RED FOX JUMPS OVER FRED'S RED FENCE.") == 0);
	str = SET(arr, 28, 27, toLowerCase);
	ASSERT("toLowerCase()", str->strcmp("a red fox jumps over fred's red fence.") == 0);
	str = ADD(arr, 29, String, "\n\t  This is a string.\n\n");
	str = SET(arr, 30, 29, trim);
	ASSERT("trim()", str->strcmp("This is a string.") == 0);

	for (int i = 0; i < 50; i++) if (arr[i] != NULL) DEL_(arr[i]);

	printf("%d/%d=%.02f%%\n", passed, (passed+failed), (100.0f*passed)/(passed+failed));
}

Object* filterNames(size_t argc, ...) {
	va_list argv;
	va_start(argv, argc);
	Object* that = (argc > 1) ? va_arg(argv, Object*) : NULL;
	Object* item = (argc > 2) ? va_arg(argv, Object*) : NULL;
	size_t ix = (argc > 3) ? va_arg(argv, size_t) : -1;
	Array* arr = (argc > 3) ? va_arg(argv, Array*) : (Array*)NULL;
	char* buf = item->toString();
	int result = strncmp(buf, "Mike");
	FREE(buf);
	return result < 0 ? True : False;
}

Object* findName(size_t argc, ...) {
	va_list argv;
	va_start(argv, argc);
	Object* that = (argc > 1) ? va_arg(argv, Object*) : NULL;
	Object* item = (argc > 2) ? va_arg(argv, Object*) : NULL;
	size_t ix = (argc > 3) ? va_arg(argv, size_t) : -1;
	Array* arr = (argc > 3) ? va_arg(argv, Array*) : (Array*)NULL;
	char* buf = item->toString();
	int result = strncmp(buf, "Mike");
	FREE(buf);
	return result == 0 ? True : False;
}

Object* getNames(size_t argc, ...) {
	va_list argv;
	va_start(argv, argc);
	String* str = va_arg(argv, String*);
	String* item = va_arg(argv, String*);
	size_t ix = (argc > 3) ? va_arg(argv, size_t) : -1;
	String* res = str->concat(item);
	*str = *res;
	DEL_(res);
	return Null;
}

Object* transformNames(size_t argc, ...) {
	va_list argv;
	va_start(argv, argc);
	va_arg(argv, String*);
	String* str = va_arg(argv, String*);
	String* res = str->toLowerCase();
	return res;
}

int reverseCompare(Object* a, Object* b) {
	return b->compareTo(a);
}

void testArrayPtr() {
	Array* arr[50];
	int passed = 0;
	int failed = 0;
	for (int i = 0; i < 50; i++) arr[i] = NULL;
	Array* array = NULL;
	String* str1 = NULL;
	String* str2 = NULL;

	array = ADD(arr, 0, Array, 3, NEW_(String, "Red"), NEW_(String, "Green"), NEW_(String, "Blue"));
	char* buf = arr[0]->toString();
	ASSERT("toString/join_(', ')", strncmp(buf, "Red,Green,Blue", strlen(buf)) == 0);
	FREE(buf);
	str1 = arr[0]->join(",");
	ASSERT("join('')", str1->strcmp("Red,Green,Blue") == 0);
	DEL_(str1);

	ADD(arr, 1, Array, 3, NEW_(String, "Rose"), NEW_(String, "Grass"), NEW_(String, "Sky"));
	array = SET(arr, 2, 0, concat, arr[1]);
	buf = array->toString();
	ASSERT("concat()", strncmp(buf, "Red,Green,Blue,Rose,Grass,Sky", strlen(buf)) == 0);
	FREE(buf);
	buf = (*array)[0]->toString();
	ASSERT("array[0] == 'Red'", strncmp(buf, "Red", strlen(buf)) == 0);
	FREE(buf);
	buf = (*array)[4]->toString();
	ASSERT("array[4] == 'Grass'", strncmp(buf, "Grass", strlen(buf)) == 0);
	FREE(buf);
	array->cleanUp();

	str1 = NEW_(String, "Dummy");
	str2 = NEW_(String, "Bakka");
	array = ADD(arr, 3, Array, 3, str1, str1, str1);
	array->fill(str2, 2, 5);
	buf = array->toString();
	ASSERT("fill('Bakka', 2, 5) == 'Dummy, Dummy, Bakka'", strncmp(buf, "Dummy,Dummy,Bakka", strlen(buf)) == 0);
	FREE(buf);
	DEL_(str1);
	DEL_(str2);
	array = ADD(arr, 4, Array, 7, NEW_(String, "Adam"), NEW_(String, "John"), NEW_(String, "Mike"), NEW_(String, "Oscar"), NEW_(String, "Thomas"), NEW_(String, "Mike"), NEW_(String, "Walter"));
	SET(arr, 5, 4, filter, filterNames);
	str1 = (String*)array->find(findName);
	buf = str1->toString();
	ASSERT("find('Mike')", strncmp(buf, "Mike") == 0);
	FREE(buf);
	ASSERT("findIndex('Mike') == 2", array->findIndex(findName) == 2);
	str2 = NEW_(String, "");
	array->forEach(getNames, str2);
	buf = str2->toString();
	ASSERT("forEach(getName)", strncmp(buf, "AdamJohnMikeOscarThomasMikeWalter") == 0);
	FREE(buf);
	DEL_(str2);
	ASSERT("indexOf('Mike') == 2", array->indexOf(str1) == 2);
	ASSERT("indexOf('Mike', 2) == 2", array->indexOf(str1, 2) == 2);
	ASSERT("indexOf('Mike', 3) == 5", array->indexOf(str1, 3) == 5);
	ASSERT("indexOf('Mike', -2) == 5", array->indexOf(str1, -2) == 5);
	ASSERT("indexOf('Mike', -5) == 2", array->indexOf(str1, -5) == 2);
	ASSERT("indexOf('Mike', -10) == 2", array->indexOf(str1, -10) == 2);
	ASSERT("lastIndexOf('Mike') == 5", array->lastIndexOf(str1) == 5);
	ASSERT("lastIndexOf('Mike', 2) == 2", array->lastIndexOf(str1, 2) == 2);
	ASSERT("lastIndexOf('Mike', 3) == 2", array->lastIndexOf(str1, 3) == 2);
	ASSERT("lastIndexOf('Mike', 6) == 5", array->lastIndexOf(str1, 6) == 5);
	ASSERT("lastIndexOf('Mike', -2) == 5", array->lastIndexOf(str1, -2) == 5);
	ASSERT("lastIndexOf('Mike', -4) == 2", array->lastIndexOf(str1, -4) == 2);
	ASSERT("lastIndexOf('Mike', -10) == -1", array->lastIndexOf(str1, -10) == -1);
	//Array* map(Function*, Object* = NULL);
	SET(arr, 6, 4, map, transformNames);
	buf = arr[6]->toString();
	ASSERT("map(transformNames)", strncmp(buf, "adam,john,mike,oscar,thomas,mike,walter") == 0);
	FREE(buf);
	arr[6]->cleanUp();
	str1 = (String*)array->pop();
	str2 = (String*)array->pop();
	DEL_(str2);
	buf = array->toString();
	ASSERT("pop()", strncmp(buf, "Adam,John,Mike,Oscar,Thomas") == 0);
	FREE(buf);
	array->push(str1);
	buf = array->toString();
	ASSERT("push('Walter')", strncmp(buf, "Adam,John,Mike,Oscar,Thomas,Walter") == 0);
	FREE(buf);
	ASSERT("push(3, ...) == 9)", array->push(3, NEW_(String, "Zelda"), NEW_(String, "Zimba"), NEW_(String, "Zork")) == 9);
	buf = array->toString();
	ASSERT("push(3, 'Zelda', 'Zimba', 'Zork')", strncmp(buf, "Adam,John,Mike,Oscar,Thomas,Walter,Zelda,Zimba,Zork", strlen(buf)) == 0);
	FREE(buf);
	array->reverse();
	buf = array->toString();
	ASSERT("reverse()", strncmp(buf, "Zork,Zimba,Zelda,Walter,Thomas,Oscar,Mike,John,Adam", strlen(buf)) == 0);
	FREE(buf);
	str1 = (String*)array->shift();
	buf = str1->toString();
	ASSERT("shift() == 'Zork'", strncmp(buf, "Zork") == 0);
	FREE(buf);
	DEL_(str1);
	SET(arr, 7, 4, slice);
	buf = arr[7]->toString();
	ASSERT("slice()", strncmp(buf, "Zimba,Zelda,Walter,Thomas,Oscar,Mike,John,Adam") == 0);
	FREE(buf);
	SET(arr, 8, 4, slice, 0, 1);
	buf = arr[8]->toString();
	ASSERT("slice(0, 2)", strncmp(buf, "Zimba,Zelda") == 0);
	FREE(buf);
	SET(arr, 9, 4, slice, 2, 3);
	buf = arr[9]->toString();
	ASSERT("slice(2, 4)", strncmp(buf, "Walter,Thomas") == 0);
	FREE(buf);
	SET(arr, 10, 4, slice, -5, -3);
	buf = arr[10]->toString();
	ASSERT("slice(-5, -3)", strncmp(buf, "Thomas,Oscar,Mike") == 0);
	FREE(buf);
	buf = (*array)[2]->toString();
	array->push(NEW_(String, buf));
	array->push(NEW_(String, "George"));
	array->sort();
	buf = arr[4]->toString();
	ASSERT("sort()", strncmp(buf, "Adam,George,John,Mike,Oscar,Thomas,Walter,Walter,Zelda,Zimba") == 0);
	FREE(buf);
	array->sort(reverseCompare);
	buf = array->toString();
	ASSERT("sort(reverseCompare)", strncmp(buf, "Zimba,Zelda,Walter,Walter,Thomas,Oscar,Mike,John,George,Adam") == 0);
	FREE(buf);
	SET(arr, 11, 4, splice, 2, 2);
	buf = arr[11]->toString();
	ASSERT("splice(2,2)", strncmp(buf, "Walter,Walter") == 0);
	FREE(buf);
	buf = array->toString();
	ASSERT("...", strncmp(buf, "Zimba,Zelda,Thomas,Oscar,Mike,John,George,Adam") == 0);
	FREE(buf);
	SET(arr, 12, 4, splice, 2, 0, 2, (*arr[11])[0], (*arr[11])[1]);
	buf = arr[12]->toString();
	ASSERT("splice(2,0,...)", strncmp(buf, "") == 0);
	FREE(buf);
	buf = array->toString();
	ASSERT("...", strncmp(buf, "Zimba,Zelda,Walter,Walter,Thomas,Oscar,Mike,John,George,Adam") == 0);
	FREE(buf);
	array->unshift(NEW_(String, "Zork"));
	buf = array->toString();
	ASSERT("unshift('Zork')", strncmp(buf, "Zork,Zimba,Zelda,Walter,Walter,Thomas,Oscar,Mike,John,George,Adam") == 0);
	FREE(buf);
	array->unshift(3, NEW_(String, "3XZ-01d"), NEW_(String, "486Cd"), NEW_(String, "900Td"));
	buf = array->toString();
	ASSERT("unshift(3, '3XZ-01d', '486Cd', '900Td')", strncmp(buf, "900Td,486Cd,3XZ-01d,Zork,Zimba,Zelda,Walter,Walter,Thomas,Oscar,Mike,John,George,Adam") == 0);
	FREE(buf);
	array->sort();
	buf = array->toString();
	printf("%s\n", buf);
	FREE(buf);
	str1 = NEW_(String, "900Td");
	long long ix = array->search(str1);
	ASSERT("search('900Td')", ix == 2);
	DEL_(str1);
	str1 = NEW_(String, "Peter");
	ix = array->search(str1);
	ASSERT("search('Peter')", ix == -9);
	SET(arr, 13, 4, splice, -ix, 0, 1, str1);
	str1 = NEW_(String, "Robert");
	ix = array->search(str1);
	ASSERT("search('Robert')", ix == -11);
	DEL_(str1);

	//size_t unshift(size_t, va_list);
	array->cleanUp();
	//array = ADD(arr, 13, Array, );


	for (int i = 0; i < 50; i++) {
		if (arr[i] != NULL) {
			DEL_(arr[i]);
		}
	}
	printf("\n%d/%d=%.02f%%\n", passed, (passed + failed), (100.0f*passed) / (passed + failed));
}

void testPathInfo() {
	size_t passed = 0;
	size_t failed = 0;

	#define path "c:\\dir\\subdir\\sub.subdir"
	#define filename "file"
	#define extension "ext"
	#define fullpath "" path "\\" filename "." extension
	PathInfo* pathInfo = NEW_(PathInfo, fullpath);
	ASSERT("PathInfo('" fullpath "').path", pathInfo->getPath()->strcmp(path) == 0);
	ASSERT("PathInfo('" fullpath "').fileName", pathInfo->getFileName()->strcmp(filename) == 0);
	ASSERT("PathInfo('" fullpath "').extension", pathInfo->getExtension()->strcmp(extension) == 0);
	DEL_(pathInfo);

	#undef fullpath
	#define fullpath "" path "\\" filename
	pathInfo = NEW_(PathInfo, fullpath);
	ASSERT("PathInfo('" fullpath "').path", pathInfo->getPath()->strcmp(path) == 0);
	ASSERT("PathInfo('" fullpath "').fileName", pathInfo->getFileName()->strcmp(filename) == 0);
	ASSERT("PathInfo('" fullpath "').extension", pathInfo->getExtension()->strcmp("") == 0);
	DEL_(pathInfo);

	#undef fullpath
	#define fullpath "" filename "." extension
	pathInfo = NEW_(PathInfo, fullpath);
	ASSERT("PathInfo('" fullpath "').path", pathInfo->getPath()->strcmp("") == 0);
	ASSERT("PathInfo('" fullpath "').fileName", pathInfo->getFileName()->strcmp(filename) == 0);
	ASSERT("PathInfo('" fullpath "').extension", pathInfo->getExtension()->strcmp(extension) == 0);
	DEL_(pathInfo);

	#undef fullpath
	#define fullpath "" filename
	pathInfo = NEW_(PathInfo, fullpath);
	ASSERT("PathInfo('" fullpath "').path", pathInfo->getPath()->strcmp("") == 0);
	ASSERT("PathInfo('" fullpath "').fileName", pathInfo->getFileName()->strcmp(filename) == 0);
ASSERT("PathInfo('" fullpath "').extension", pathInfo->getExtension()->strcmp("") == 0);
DEL_(pathInfo);

#undef path
#undef fullpath
#define path "/dir/subdir/sub.subdir"
#define filename "file"
#define extension "ext"
#define fullpath "" path "/" filename "." extension
pathInfo = NEW_(PathInfo, fullpath);
ASSERT("PathInfo('" fullpath "').path", pathInfo->getPath()->strcmp(path) == 0);
ASSERT("PathInfo('" fullpath "').fileName", pathInfo->getFileName()->strcmp(filename) == 0);
ASSERT("PathInfo('" fullpath "').extension", pathInfo->getExtension()->strcmp(extension) == 0);
DEL_(pathInfo);


printf("\n%lld/%lld=%.02f%%\n", passed, (passed + failed), (100.0f*passed) / (passed + failed));
}

void testMapPtr() {
	int passed = 0;
	int failed = 0;

	Map* map = NULL;
	Array* keys = NEW_(Array, 3, NEW_(String, "k1"), NEW_(String, "k2"), NEW_(String, "k3"));
	Array* values = NEW_(Array, 3, NEW_(String, "v1"), NEW_(String, "v2"), NEW_(String, "v3"));
	char* buf = NULL;

	map = NEW_(Map);
	map->put(keys->get(0), values->get(0));
	map->put(keys->get(1), values->get(1));
	map->put(keys->get(2), values->get(2));
	buf = map->get(keys->get(0))->toString();
	ASSERT("put(k1, v1) / get(k1)", values->get(0)->strcmp(buf) == 0);
	FREE(buf);
	buf = map->get(keys->get(1))->toString();
	ASSERT("put(k2, v2) / get(k2)", values->get(1)->strcmp(buf) == 0);
	FREE(buf);
	buf = map->get(keys->get(2))->toString();
	ASSERT("put(k3, v3) / get(k3)", values->get(2)->strcmp(buf) == 0);
	FREE(buf);
	map->put(keys->get(1), values->get(2));
	buf = map->get(keys->get(1))->toString();
	ASSERT("put(k2, v3) / get(k2)", values->get(2)->strcmp(buf) == 0);
	FREE(buf);
	buf = map->keys()->toString();
	ASSERT("keys = [k1,k2,k3]", strncmp(buf, "k1,k2,k3") == 0);
	FREE(buf);
	buf = map->values()->toString();
	ASSERT("values = [v1,V3,v3]", strncmp(buf, "v1,v3,v3") == 0);
	FREE(buf);
	map->cleanUp();
	DEL_(map);
	keys->cleanUp();
	values->cleanUp();
	DEL_(keys);
	DEL_(values);

	Array* arr = NEW_(Array, 4, NEW_(String, "k1=v1"), NEW_(String, "k2=v2"), NEW_(String, "k3=v3"), NEW_(String, "k4"));
	String* sep = NEW_(String, "=");
	map = NEW_(Map, arr, sep);
	DEL_(sep);
	arr->cleanUp();
	DEL_(arr);
	map->keys()->cleanUp();
	map->values()->cleanUp();
	map->cleanUp();
	DEL_(map);

printf("\n%d/%d=%.02f%%\n", passed, (passed + failed), (100.0f*passed) / (passed + failed));

}

Object* testTreePre(size_t count, ...) {
	va_list args;
	va_start(args, count);
	Node* node = va_arg(args, Node*);
	Array* output = va_arg(args, Array*);
	output->push(NEW_(String, node->value()->toString()));
	return NULL;
}
Object* testTreeIn(size_t count, ...) {
	va_list args;
	va_start(args, count);
	Edge* edge = va_arg(args, Edge*);
	Array* output = va_arg(args, Array*);
	output->push(NEW_(String, "-["));
	output->push(NEW_(String, edge->value()->toString()));
	output->push(NEW_(String, "]->"));
	return NULL;
}
Object* testTreePost(size_t count, ...) {
	va_list args;
	va_start(args, count);
	Node* node = va_arg(args, Node*);
	Array* output = va_arg(args, Array*);
	if (node->edges()->size() == 0) {
		output->push(NEW_(String, "\n"));
	}
	return NULL;
}

void testTreePtr() {
	int passed = 0;
	int failed = 0;

	Tree* tree = NEW_(Tree);
	tree->addNode(NULL, NEW_(String, "Sándor"));
	Node* node = tree->addNode(tree->root(), NEW_(String, "Gabor"), NEW_(String, "child"));
	tree->addNode(node, NEW_(String, "Gergöke"), NEW_(String, "child"));
	node = tree->addNode(tree->root(), NEW_(String, "Adri"), NEW_(String, "child"));
	tree->addNode(node, NEW_(String, "Klárika"), NEW_(String, "child"));
	tree->addNode(node, NEW_(String, "Jancsika"), NEW_(String, "child"));
	Array* output = NEW_(Array);
	tree->traverseDFS(testTreePre, testTreeIn, testTreePost, output);
	String* str = output->join("");
	char* buf = str->toString();
	printf("%s\n", buf);
	FREE(buf);
	DEL_(str);
	output->cleanUp();
	DEL_(output);
	tree->cleanUp();
	DEL_(tree);

	printf("\n%d/%d=%.02f%%\n", passed, (passed + failed), (100.0f*passed) / (passed + failed));
}

size_t primeTest(size_t n) {
	size_t divider = 0;
	size_t root = (size_t)sqrt((double)n);
	for (size_t i = 2; i <= root; i++) {
		if ((n % i) == 0) {
			divider = i;
			break;
		}
	}
	return divider;
}

void findPrime(size_t n) {
	while (true) {
		printf("%llx is ", n);
		size_t i = primeTest(n);
		if (i == 0) {
			printf("a prime.\n");
			break;
		}
		printf("dividable by %llu.\n", i);
		n++;
	}
}

int _main(NS_FW_BASE::Map* args) {
	int error = 0;
	for (int i = 0; i < args->keys()->length(); i++) {
		char* key = args->keys()->get(i)->toString();
		char* value = args->values()->get(i)->toString();
		printf("%s = %s\n", key, value);
		DEL_(key);
		DEL_(value);
	}

	size_t bitmask = 256;
	for (int i = 8; i < 64; i++) {
		size_t t = bitmask - 1;
		bitmask <<= 1;
		for (size_t j = t; j < bitmask - 1; j++) {
			if ((j - 3) % 4 == 0 && primeTest(j) == 0) {
				printf("%2.d bits: %16.lld 0x%016llX\n", i, j, j);
				break;
			}
		}
	}

	//findPrime(0x9FFFFFFFFFFFFFULL);

	//printf("\n*** Object tests\n");
	//testObjectPtr();
	//printf("*** String tests\n");
	//testStringPtr();

	//printf("\n*** Array tests\n");
	//testArrayPtr();

	//printf("\n*** Map tests\n");
	//testMapPtr();

	//printf("\n*** Tree tests\n");
	//testTreePtr();

	//printf("\n*** PathInfo tests\n");
	//testPathInfo();

	//const size_t length = 60 * 48000;
	//Buffer buffer(TYPE_FLOAT, length);
	//for (int i = 0; i < length; i++) {
	//	float rnd = (float)Utils::randomSigned();
	//	buffer.push(rnd);
	//}
	//File::write("samplesb.smp", buffer);
	return error;
}