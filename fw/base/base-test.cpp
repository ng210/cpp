#include "base/base-test.h"
#include "base/memory.h"
#include "base/debug.h"
#include "base/str.h"
#include "base/string.h"

NS_FW_BASE_USE

void BaseTest::strTests() {
	const char* src = "  Hello world!  ";
#ifdef _CRT
	assert("strspn(' Helo') should return 8", strspn(src, " Helo") == 8);
	assert("strcspn('lo') should return 4", strcspn(src, "lo") == 4);
#endif
	assert("strrspn(' rld!') should return 10", strrspn(src, " rld!") == 10);
	assert("strrcspn('Hello') should return 11", strrcspn(src, "Hello") == 11);
}

void BaseTest::stringTests() {
	char* str = NULL;
	str = str_concat("Hello", "world");
	assert("concat should create 'Helloworld", strncmp(str, "Helloworld") == 0);
	FREE(str);
	str = str_concat(4, "Concat test: ", "Hello", " ", "world!");
	assert("concat should create 'Concat test: Hello world!'", strncmp(str, "Concat test: Hello world!") == 0);
	FREE(str);
	assert("string should end with 'world!'", str_ends("Hello world!", "world!") != 0);
	assert("string should not end with 'qorld!'", str_ends("Hello world!", "qorld!") == 0);
	assert("string should start with 'Hello'", str_starts("Hello world!", "Hello") != 0);
	assert("string should not start with 'Bello'", str_starts("Hello world!", "Bello") == 0);
	const char* term1 = "The red fox jumps over a red fence.";
	assert("indexOf 'red' in 'The red fox jumps over a red fence.' should be 4", str_indexOf(term1, "red") == 4);
	assert("indexOf 'red' in 'The red fox jumps over a red fence.' starting at 8 should be 25", str_indexOf(term1, "red", 8) == 25);
	assert("indexOf 'red' in 'The red fox jumps over a red fence.' starting at 26 should be -1", str_indexOf(term1, "Hello", 2) == -1);
	assert("lastIndexOf 'red' in 'The red fox jumps over a red fence.' should be 25", str_lastIndexOf(term1, "red") == 25);
	assert("lastIndexOf 'blue' in 'The red fox jumps over a red fence.' should be -1", str_lastIndexOf(term1, "blue") == -1);
	str = str_join(4, " - ", "Join", "test", "Hello", "world!");
	assert("join should return 'Join - test - Hello - world!", strncmp(str, "Join - test - Hello - world!") == 0);
	FREE(str);
	str = str_replace(term1, "red", "blue");
	assert("replace should return 'The blue fox jumps over a blue fence.'", strncmp(str, "The blue fox jumps over a blue fence.") == 0);
	FREE(str);
	str = str_replace(term1, "red", "x");
	assert("replace should return 'The x fox jumps over a x fence.'", strncmp(str, "The x fox jumps over a x fence.") == 0);
	FREE(str);
	str = str_replace(term1, "red ", "");
	assert("replace should return 'The fox jumps over a fence.'", strncmp(str, "The fox jumps over a fence.") == 0);
	FREE(str);
	str = str_substring(term1, 12);
	assert("substr(12) should return 'jumps over a red fence.'", strncmp(str, "jumps over a red fence.") == 0);
	FREE(str);
	str = str_substring(term1, 12, 17);
	assert("substr(12, 17) should return 'jumps'", strncmp(str, "jumps") == 0);
	FREE(str);
	str = str_substring(term1, 12, -7);
	assert("substr(12, -7) should return 'jumps over a red'", strncmp(str, "jumps over a red") == 0);
	FREE(str);
	str = str_substring(term1, -17, -18);
	assert("substr(-17, -18) should return 'The red fox jumps'", strncmp(str, "The red fox jumps") == 0);
	FREE(str);
	str = str_substring(term1, 4, 17);
	assert("substr(4, 17) should return 'red fox jumps'", strncmp(str, "red fox jumps") == 0);
	FREE(str);
	str = str_substring(term1, 17, 4);
	assert("substr(17, 4) should return 'red fox jumps'", strncmp(str, "red fox jumps") == 0);
	FREE(str);
	str = str_substring(term1, -107, -111);
	assert("substr(-107, -111) should return ''", strncmp(str, "") == 0);
	FREE(str);
	char** arr = str_split(term1, "red");
	assert("split should return at #0 'The '", strncmp(arr[0], "The ") == 0);
	assert("split should return at #1 ' fox jumps over a '", strncmp(arr[1], " fox jumps over a ") == 0);
	assert("split should return at #2 ' fence.'", strncmp(arr[2], " fence.") == 0);
	assert("split should return at #3 NULL", arr[3] == NULL);
	str = str_join(arr, "red");
	assert("join of split results should return the original string", strncmp(str, term1) == 0);
	FREE(str);
	for (int i = 0; (str = arr[i]) != NULL; i++) {
		FREE(str);
	}
	FREE(arr);
	arr = str_split(" Hello world! ", " ");
	assert("split should return at #0 'Hello'", strncmp(arr[0], "Hello") == 0);
	assert("split should return at #1 'world!'", strncmp(arr[1], "world!") == 0);
	for (int i = 0; (str = arr[i]) != NULL; i++) {
		FREE(str);
	}
	FREE(arr);

	const char* term2 = " \t Hello \t world!\n \t ";
	str = str_ltrim(term2);
	assert("ltrim should return 'Hello \\t world!\\n \\t ", strncmp(str, "Hello \t world!\n \t ") == 0);
	FREE(str);
	str = str_rtrim(term2);
	assert("rtrim should return ' \\t Hello \\t world!", strncmp(str, " \t Hello \t world!") == 0);
	FREE(str);
	str = str_trim(term2);
	assert("trim should return 'Hello \\t world!", strncmp(str, "Hello \t world!") == 0);
	FREE(str);
}
