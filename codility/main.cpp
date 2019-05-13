/******************************************************************************
Task
Given an arbitrary large text file (few terrabytes) containing arbitrary
big numbers each line, calculate the median of these numbers.
Solution contains 2 features
 - BigFloat: a class of float numbers with predefined precision (digit count)
 - buffering to process the text file effectively

 Buffering
 A memory buffer is used to read the file piecewise. The lines in the buffer
 are fetched into a secondary line-buffer. The input numbers are parsed from
 this line buffer. If the main buffer runs out before the line is terminated,
 it will be re-filled from the file and the line-buffer can be filled up
 continuously.

 Tests
 The procedure 'tests' executes the basic operation on the BigFloat type.
 Addition (subtraction included), multiplication and division.
 Due to the implementation of BigFloat these operations have to be tested
 with both order of the operands (e.g. a + b and b + a).

 MicroDoc
 The procedure 'microDoc' opens the file referenced by the path as its argument
 and processes the file: reads and parses each line, accumulates the sum in the
 variable 'total' and the count of read numbers in the variable 'count'.
 Finally, it calculates the median and prints the results.

******************************************************************************/
#include <stdio.h>
#include "bigfloat.h"

#define BUFFER_SIZE 4096
#define LINE_SIZE 256

#define PRINT(t, n) { char* __print_str = n.toString(0); printf(t " %s\n", __print_str); delete[]__print_str; }
#define DBG(t, n) { char* __print_str = n.toString(0); printf(t " %s(%d)\n", __print_str, n.length()); delete[]__print_str; }

// few primary tests
void tests() {
	// test1
	BigFloat a("123.25");
	BigFloat b("0.025");
	BigFloat c("-0.025");
	DBG("a=", a);
	DBG("b=", b);
	DBG("c=", c);
	// test2
	c = a + b;
	DBG("a+b=", c);
	c = b + a;
	DBG("b+a=", c);
	c = a * b;
	DBG("a*b=", c);
	c = b * a;
	DBG("b*a=", c);
	c = a / b;
	DBG("a/b=", c);
	c = b / a;
	DBG("b/a=", c);
}

// create test data file
void createTestData(char** pFileName) {
	*pFileName = (char*)"test.txt";
	long long seed = -1234567890123456789;
	FILE* fp = fopen(*pFileName, "w");
	for (int i = 0; i < 5000; i++) {
		fprintf(fp, "%lld\n", seed);
		seed = seed + 876543210987654321;
	}
	fclose(fp);
}

// run task
void microDoc(const char* fileName) {
	// open file to read
	FILE* fp = fopen(fileName, "rb");

	// allocate working buffer of predefined size
	char c, *buffer = new char[BUFFER_SIZE];
	char line[LINE_SIZE];
	int bufferIx, lineIx = 0, lineNumber = 1;
	int skip = 0;
	size_t length;
	BigFloat total;
	BigFloat count;

	do {
		// fill buffer
		length = fread(buffer, 1, BUFFER_SIZE, fp);
		bufferIx = 0;

		// process buffer
		while (bufferIx < length) {
			c = buffer[bufferIx++];
			// read to end of line (CR LF or NL)
			if (c == '\n' || c == '\r') {
				if (!skip) {
					c = buffer[bufferIx];
					// skip CR LF (Windows)
					if (c == '\n' || c == '\r') {
						bufferIx++;
					}
					if (lineIx > 0) {
						// calculate sum
						line[lineIx++] = '\0';
						BigFloat operand(line);
						//PRINT("operand=", operand);
						total += operand;
						//PRINT("total  =", total);
						count++;
						lineIx = 0;
					}
					lineNumber++;
				} else {
					// skip line
					while (bufferIx < length) {
						c = buffer[bufferIx++];
						if (c != '\n' && c != '\r') {
							lineNumber++;
							lineIx = 0;
							skip = 0;
							break;
						}
					}
				}
			} else {
				if (!skip) {
					line[lineIx++] = c;
					if (lineIx == LINE_SIZE) {
						printf("Overflow at %d.\n", lineNumber);
						skip = 1;
					}
				}
			}
		}
	} while (length == BUFFER_SIZE);
	fclose(fp);
	PRINT("Sum=", total);
	PRINT("Count=", count);
	BigFloat median(total / count);
	PRINT("Median=", median);
}

int main(int argc, const char** argv) {
	char* fileName;
	if (argc == 1) {
		createTestData(&fileName);
	} else {
		fileName = (char*)argv[1];
	}

	// BigFloat tests
	//tests();

	// calculate median
	microDoc(fileName);

	return 0;
}
