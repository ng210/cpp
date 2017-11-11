#include <stdio.h>
//#include <stdlib.h>
//#include "base/str.h"

#define MEM_DEBUG

#include "consoleapp.h"

//#include <stdarg.h>

NS_FW_BASE_USE

//extern int _main(NS_FW_BASE::Array*);

int g_id = 1;

//#define STRINGIFY(x) #x
#define ASSERT(label, expr) printf(" - " label " "); if (expr) { passed++; printf("passed\n"); } else { failed++; printf("failed\n"); };
#define ADD(arr, ix, type, ...) arr[ix] = NEW_(type, ##__VA_ARGS__)
#define SET(arr, dst, src, fn, ...) arr[dst] = arr[src]->##fn(##__VA_ARGS__)

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
	ASSERT("Substring(-5, 6)", str->strcmp("A red fox") == 0);
	str = SET(arr, 17, 8, substring, -22, 8);
	ASSERT("Substring(-22, 8)", str->strcmp("A red fox") == 0);
	str = SET(arr, 18, 8, substring, 2, -7);
	ASSERT("Substring(2, -7)", str->strcmp("red fox jumps over Fred's red") == 0);
	str = SET(arr, 19, 8, substring, 2, 6);
	ASSERT("Substr(2, 6)", str->strcmp("red fox") == 0);
	str = SET(arr, 20, 8, substring, -5, 9);
	ASSERT("Substr(-5, 6)", str->strcmp("A red fox") == 0);
	str = SET(arr, 21, 8, substring, -22, 8);
	ASSERT("Substr(-22, 8)", str->strcmp("A red fox") == 0);
	str = SET(arr, 22, 8, substring, 2, -7);
	ASSERT("Substr(2, -7)", str->strcmp("red fox jumps over Fred's red") == 0);

	String** tokens = arr[8]->split(" ");
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
	int result = strncmp(buf, "Michael", strlen(buf));
	FREE(buf);
	return result < 0 ? True : False;
}

//#define ARR(ix, expr) arr[ix] = expr; { const char* __str = arr[ix]->join_(", "); printf("%02d. '%s'\n", g_id++, __str); FREE(__str); }
//#define ARR1(dst, src, fn, ix) arr[dst] = arr[src]->##fn(arr[ix]); { \
//	const char* __str1 = arr[src]->join_(", "); \
//	const char* __str2 = arr[ix]->join_(", "); \
//	const char* __str3 = arr[dst]->join_(", "); \
//	printf("%02d. '%s'->" STRINGIFY(fn) "('%s')='%s'\n", g_id++, __str1, __str2, __str3); FREE(__str1); FREE(__str2); FREE(__str3); }

//Object* cleanupArray(size_t count, ...) {
//	va_list args;
//	va_start(args, count);
//	Object* that = va_arg(args, Object*);
//	Object* item = va_arg(args, Object*);
//	size_t i = va_arg(args, size_t);
//	Array* arr = va_arg(args, Array*);
//	DEL_(item);
//	return NULL;
//}

void testArray() {
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
	str1 = arr[0]->join("");
	ASSERT("join('')", str1->strcmp("RedGreenBlue") == 0);
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

	//void fill(Object*, size_t = 0, size_t = 0);
	str1 = NEW_(String, "Dummy");
	str2 = NEW_(String, "Bakka");
	array = ADD(arr, 3, Array, 3, str1, str1, str1);
	array->fill(str2, 2,5);
	buf = array->toString();
	ASSERT("fill('Bakka', 2, 5) == 'Dummy, Dummy, Bakka'", strncmp(buf, "Dummy,Dummy,Bakka", strlen(buf)) == 0);
	FREE(buf);
	DEL_(str1);
	DEL_(str2);
	array = ADD(arr, 4, Array, 5, NEW_(String, "Adam"), NEW_(String, "John"), NEW_(String, "Mike"), NEW_(String, "Oscar"), NEW_(String, "Thomas"));
	SET(arr, 5, 4, filter, filterNames);
	array->cleanUp();
	//Array* filter(Function*, Object* = NULL);
	//Object* find(Function*, Object* = NULL);
	//long long findIndex(Function*, Object* = NULL);
	//void forEach(Function*, Object* = NULL);
	//long long indexOf(Object*, size_t = 0);
	//long long lastIndexOf(Object*, size_t = -1);
	//Array* map(Function*, Object* = NULL);
	//Object* pop();
	//size_t push(Object*);
	//size_t push(size_t, ...);
	//size_t push(size_t, va_list);
	//Array* reverse();
	//Object* shift();
	//Array* slice(long long = 0, long long = 0);
	//void sort(Function*);
	//Array* splice(long long, size_t, size_t = 0, ...);
	//Array* splice(long long, size_t, size_t, va_list);
	//size_t unshift(Object*);
	//size_t unshift(size_t, ...);
	//size_t unshift(size_t, va_list);

	for (int i = 0; i < 50; i++) {
		if (arr[i] != NULL) {
			DEL_(arr[i]);
		}
	}
	printf("\n%d/%d=%.02f%%\n", passed, (passed + failed), (100.0f*passed) / (passed + failed));
}

//Object myCallback(size_t argc, ...) {
//	return null;
//}

int _main(NS_FW_BASE::Array* args) {
	int error = 0;

	printf("*** String tests\n");
	testStringPtr();
	printf("\n*** Array tests\n");
	testArray();

	return error;
}