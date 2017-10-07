#include <stdio.h>
//#include <stdlib.h>
#include "base/str.h"
#include "consoleapp.h"
#include "base/MemoryMgr.h"

NAMESPACE_FRMWRK_USE

void test() {
	String str0 = null;
	str0 = null;
	Object obj = null;
	obj = null;
	String str1 = "AbCdEf";
	String str2 = "GhIjKl";
	String str3 = "AbMnOpQr";
	String str = str1 + "_" + str2 + " " + str3;
	printf("1. str3=str1+\"_\"+str2+\" \"='%s'\n", str.toCString());
	printf("2.1 str3.charAt(2)='%c'\n", str[2]);
	printf("2.2 str.charAt(-1)='%c'\n", str[-1]);
	printf("2.3 str.charAt(25)='%c'\n", str[25]);
	printf("3.1 str.indexOf(str1)=%lld\n", str.indexOf(str1));
	printf("3.2 str.indexOf(str2)=%lld\n", str.indexOf(str2));
	printf("3.3 str.indexOf(str3)=%lld\n", str.indexOf(str3));
	printf("3.4 str.indexOf('Ab', 5)=%lld\n", str.indexOf("Ab", 5));
	printf("4.1 str.lastIndexOf(str1)=%lld\n", str.lastIndexOf(str1));
	printf("4.2 str.lastIndexOf(str2)=%lld\n", str.lastIndexOf(str2));
	printf("4.3 str.lastIndexOf(str3)=%lld\n", str.lastIndexOf(str3));
	printf("4.4 str.lastIndexOf(\"Ef\")=%lld\n", str.lastIndexOf("Ef"));
	printf("5. str.startsWith('str1')='%s'\n", str.startsWith(str1) ? "true" : "false");
	printf("6. str.startsWith('str2')='%s'\n", str.startsWith(str2) ? "true" : "false");
	printf("7. str.endsWith('str2')='%s'\n", str.endsWith(str2) ? "true" : "false");
	printf("7. str.endsWith('str3')='%s'\n", str.endsWith(str3) ? "true" : "false");
	printf("8.1 str.substring(2,6)='%s'\n", str.substring(2,6).toCString());
	printf("8.2 str.substring(-2,6)='%s'\n", str.substring(-2, 6).toCString());
	printf("8.3 str.substring(-22,6)='%s'\n", str.substring(-22, 6).toCString());
	printf("8.4 str.substring(2,-6)='%s'\n", str.substring(2, -6).toCString());
	printf("8.5 str.substring(2)='%s'\n", str.substring(2).toCString());
	printf("9.1 str.substr(2,6)='%s'\n", str.substr(2, 6).toCString());
	printf("9.2 str.substr(-2,6)='%s'\n", str.substr(-2, 6).toCString());
	printf("9.3 str.substr(-22,6)='%s'\n", str.substr(-22, 6).toCString());
	printf("9.4 str.substr(2,-6)='%s'\n", str.substr(2, -6).toCString());
	printf("9.5 str.substr(2)='%s'\n", str.substr(2).toCString());
	String str4 = "\t\n";
	str = str4 + str + str4;
	printf("10.1 str.trim()='%s'\n", str.trim().toCString());
	printf("10.2 String(\"  a  \").trim()='%s'\n", String("  a  ").trim().toCString());
	printf("10.3 String(\"  \").trim()='%s'\n", String("  ").trim().toCString());
	str = "Is this the 1st test string?";
	printf("11. str.toLowerCase()='%s'\n", str.toLowerCase().toCString());
	printf("12. str.toUpperCase()='%s'\n", str.toUpperCase().toCString());
	str = "A red fox jumps over Fred's red fence.";
	String str5 = str.replace("red", "small");
	printf("13.1 ('%s').replace('red', 'small')='%s'\n", str.toCString(), str5.toCString());
	printf("13.2 ('%s').replace('red', 'silver')='%s'\n", str5.toCString(), str5.replace("red", "blue").toCString());
}

int main(int argc, char *argv[]) {
	int error = 0;

	//MemoryMgr::isDebugOn = true;

	printf("\n\n***\n");
	test();

	MemoryMgr::checkMemDbgInfo();
	printf("***\n\n\n");

	return error;
}