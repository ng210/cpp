#include <stdio.h>
//#include <stdlib.h>
#include "base/String.h"
#include "collection/Array.h"
//#include "base/consoleapp.h"
#include "base/MemoryMgr.h"

//#include <stdarg.h>

NS_FW_BASE_USE

int g_id = 1;

#define STRINGIFY(x) #x
#define STR(ix, expr) str[ix] = expr; printf("%02d. '%s'\n", g_id++, str[ix]->toString())
#define STR2(dst, src, expr) str[dst] = str[src]->expr; printf("%02d. '%s'->" STRINGIFY(expr) "='%s'\n", g_id++, str[src]->toString(), str[dst]->##toString())
#define STR3(src, fn, s, ix, f) printf("%02d. '%s'->" STRINGIFY(fn) "('%s')=" STRINGIFY(f) "\n", g_id++, str[src]->toString(), s[ix]->toString(), str[src]->##fn(s[ix]))

#define STR20(dst, src, fn, c) str[dst] = str[src]->##fn(c); \
printf("%02d. '%s'->" STRINGIFY(fn) "('" STRINGIFY(c) "')='%s'\n", g_id++, str[src]->toString(), str[dst]->##toString())
#define STR21(dst, src, fn, s, ix) str[dst] = str[src]->##fn(s[ix]); \
printf("%02d. '%s'->" STRINGIFY(fn) "('%s')='%s'\n", g_id++, str[src]->toString(), s[ix]->toString(), str[dst]->##toString())
#define STR30(src, fn, c, f) printf("%02d. '%s'->" STRINGIFY(fn) "(" STRINGIFY(c) ")=" STRINGIFY(f) "\n", g_id++, str[src]->toString(), str[src]->##fn(c))

void testStringPtr() {
	String* str[50];
	for (int i = 0; i < 50; i++) str[i] = NULL;
	size_t i = 0;
	STR(0, NEW_(String, "AbCdEf"));
	STR20(1, 0, concat, "_");
	STR(2, NEW_(String, "GhIjKl"));
	STR21(3, 1, concat, str, 2);
	STR(4, NEW_(String, "AbMnOpQr"));
	STR20(5, 3, concat, " ");
	STR(6, NEW_(String, "Baka"));
	STR21(7, 5, concat, str, 4);
	printf("%02d. '%s'[2]='%c'\n", g_id++, str[7]->toString(), (*str[7])[2]);
	printf("%02d. '%s'[-1]='%c'\n", g_id++, str[7]->toString(), (*str[7])[-1]);
	printf("%02d. '%s'[25]='%c'\n", g_id++, str[7]->toString(), (*str[7])[25]);
	STR3(7, indexOf, str, 1, %lld);
	STR3(7, indexOf, str, 2, %lld);
	STR3(7, indexOf, str, 3, %lld);
	STR3(7, indexOf, str, 6, %lld);
	STR3(7, indexOf, str, 7, %lld);
	STR30(7, indexOf, "Ab", %lld);
	STR30(7, indexOf, "Ba", %lld);
	STR3(7, lastIndexOf, str, 1, %lld);
	STR3(7, lastIndexOf, str, 2, %lld);
	STR3(7, lastIndexOf, str, 3, %lld);
	STR3(7, lastIndexOf, str, 6, %lld);
	STR3(7, lastIndexOf, str, 7, %lld);
	STR30(7, lastIndexOf, "Ef", %lld);
	STR30(7, lastIndexOf, "Ba", %lld);
	STR3(7, startsWith, str, 1, %d);
	STR3(7, startsWith, str, 2, %d);
	STR3(7, endsWith, str, 2, %d);
	STR3(7, endsWith, str, 4, %d);
	STR2(8, 7, substring(2, 6));
	STR2(9, 7, substring(-2, 6));
	STR2(10, 7, substring(-22, 6));
	STR2(11, 7, substring(2, -6));
	STR2(12, 7, substring(2));
	STR2(13, 7, substring(2, 6));
	STR2(14, 7, substr(2, 6));
	STR2(15, 7, substr(-2, 6));
	STR2(16, 7, substr(-22, 6));
	STR2(17, 7, substr(2, -6));
	STR(18, NEW_(String, "\t\n"));
	STR21(19, 18, concat, str, 7);
	STR21(20, 19, concat, str ,17);
	STR2(21, 20, trim());
	STR(22, NEW_(String, "  a  "));
	STR2(23, 22, trim());
	STR(24, NEW_(String, "  "));
	STR2(25, 24, trim());
	STR(26, NEW_(String, "Is this the 1st test string?"));
	STR2(27, 26, toLowerCase());
	STR2(28, 26, toUpperCase());
	STR(29, NEW_(String, "A red fox jumps over Fred's red fence."));
	STR2(30, 29, replace("red", "small"));
	STR2(31, 30, replace("red", "blue"));
	STR2(32, 31, replace("small", "red"));

	for (int i = 0; i < 50; i++) if (str[i] != NULL) DEL_(str[i]);
}

//Object myFilter1(size_t argc, ...) {
//	va_list argv;
//	va_start(argv, argc);
//	Object* that = (argc > 1) ? va_arg(argv, Object*) : NULL;
//	Object* item = (argc > 2) ? va_arg(argv, Object*) : NULL;
//	size_t ix = (argc > 3) ? va_arg(argv, size_t) : -1;
//	Array* arr = (argc > 3) ? va_arg(argv, Array*) : (Array*)&null;
//	return null;
//}


#define ARR(ix, expr) arr[ix] = expr; { const char* __str = arr[ix]->join_(", "); printf("%02d. '%s'\n", g_id++, __str); FREE(__str); }
#define ARR1(dst, src, fn, ix) arr[dst] = arr[src]->##fn(arr[ix]); { \
	const char* __str1 = arr[src]->join_(", "); \
	const char* __str2 = arr[ix]->join_(", "); \
	const char* __str3 = arr[dst]->join_(", "); \
	printf("%02d. '%s'->" STRINGIFY(fn) "('%s')='%s'\n", g_id++, __str1, __str2, __str3); FREE(__str1); FREE(__str2); FREE(__str3); }

Object* cleanupArray(size_t count, ...) {
	va_list args;
	va_start(args, count);
	Object* that = va_arg(args, Object*);
	Object* item = va_arg(args, Object*);
	size_t i = va_arg(args, size_t);
	Array* arr = va_arg(args, Array*);
	DEL_(item);
	return NULL;
}

void testArray() {
	Array* all = NULL;	// NEW_(Array);
	Array* arr[50];
	for (int i = 0; i < 50; i++) arr[i] = NULL;

	ARR(0, NEW_(Array, 3, NEW_(String, "Red"), NEW_(String, "Green"), NEW_(String, "Blue")));
	ARR(1, NEW_(Array, 3, NEW_(String, "Rose"), NEW_(String, "Grass"), NEW_(String, "Sky")));
	ARR1(2, 0, concat, 1);
	all = arr[2];

	all->forEach(cleanupArray);

	//printf("1. ([%s]) + ([%s])=[%s]\n", arr1.toString().toCString(), arr2.toString().toCString(), arr3.toString().toCString());
	//printf("2. arr3[3]='%s'\n", arr3[3].toString().toCString());
	//Array& fill(Object&, size_t = 0, size_t = 0);
	//Array arr4 = arr3.filter(myFilter1, arr3);
	//printf("3. arr4.filter(myFilter1)='%s'\n", arr4.toString().toCString());

	//Object find(Function&, Object& = (Object&)null);
	//long long findIndex(Function&, Object& = (Object&)null);
	//void forEach(Function&, Object& = (Object&)null);
	//long long indexOf(Object&, size_t = 0);

	//String str = arr3.join(String(" - "));
	//printf("9. arr.join='%s'\n", str.toCString());


	//long long lastIndexOf(Object&, size_t = -1);
	//Array map(Function&, Object& = (Object&)null);
	//Object pop();
	//void push(Object&);
	//void push(size_t, ...);
	//Array& reverse();
	//Object shift();
	//Array slice(long long = 0, long long = 0);
	//Array& sort(Function&);
	//Array splice(long long, size_t, size_t = 0, ...);
	//size_t unshift(Object&);
	//size_t unshift(size_t, ...);

	for (int i = 0; i < 50; i++) if (arr[i] != NULL) DEL_(arr[i]);
}

//Object myCallback(size_t argc, ...) {
//	return null;
//}

int main(int argc, char *argv[]) {
	int error = 0;

	//MemoryMgr::isDebugOn = true;

	printf("********\n");
	//testStringPtr();
	testArray();
	printf("********\n\n");

	MemoryMgr::checkMemDbgInfo();

	return error;
}