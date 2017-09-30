#include <stdio.h>
#include <stdlib.h>

#include "consoleapp.h"

NAMESPACE_FRMWRK_USE

int main(int argc, char *argv[]) {
	int error = 0;

	printf("Allocate 2 objects\n");
	Object obj1;
	Object obj2;

	printf("Object1: %s\n", (const char*)obj1);
	printf("Object2: %s\n", (const char*)obj2);

	printf("obj2 = obj1\n");
	obj2 = obj1;

	printf("Object1: %s\n", (const char*)obj1);
	printf("Object2: %s\n", (const char*)obj2);

	return error;
}