#include <stdio.h>
#include <malloc.h>

long long _binSearch(int* list, int key, size_t min, size_t max) {
	long long res = 0;
	// the search range gets halved each iteration
	while (min < max) {
		// mid = min + (max - min)/2 = min/2 + max/2
		size_t mid = (min + max) >> 1;
		// compare item with middle item
		int i = key - list[mid];

		if (i == 0) {
			// on equation the middle item was the searched item
			return mid;
		}
		if (i < 0) {
			// item was less, continue with the first half-range: [min, mid]
			max = mid;
		}
		else {
			// item was greater, continue with the second half-range: [mid+1, max]
			min = mid + 1;
		}
	}
	// item not found, negative index marks the place where the item should be.
	return -(long long)max - 1;
}

long long _linearSearch(int* list, size_t count, int key) {
	for (size_t i = 0; i < count; i++) {
		if (list[i] == key) {
			return i;
		}
	}
	return -1;
}

#define SIZE 64

typedef struct AdapterInfo {
	const char* name;
	int id;
	AdapterInfo(const char n[], int i) { name = n; id = i; }
} AdapterInfo;


void stuff1() {
	long long sum = 0;

	int* list = (int*)malloc(sizeof(int) * SIZE);
	for (int i = 0; i < SIZE; i++) {
		list[i] = i;
	}
	for (int i = 0; i < 1024 * 1024; i++) {
		for (int j = 0; j < SIZE; j++) {
			sum += _binSearch(list, j, 0, SIZE);
		}
	}
	printf("%lld\n", sum);
	free(list);

	list = (int*)malloc(sizeof(int) * SIZE);
	for (int i = 0; i < SIZE; i++) {
		list[i] = i;
	}

	sum = 0;
	for (int i = 0; i < 1024 * 1024; i++) {
		for (int j = 0; j < SIZE; j++) {
			sum += _linearSearch(list, SIZE, j);
		}
	}
	printf("%lld\n", sum);
	free(list);
}


void stuff2() {

	AdapterInfo info("Hello", 0);

	printf("%s", info.name);
}

int main(int argc, char *argv[]) {
	//stuff1();
	stuff2();
	return 0;
}