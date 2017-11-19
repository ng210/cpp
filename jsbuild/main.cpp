#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/PathInfo.h"

class Source {
	char name[256];
	Source *includes[100];
	size_t includeCount;
	size_t flag;
public:
	Source(void) {
		init("");
	}
	Source(const char* name) {
		init(name);
	}
	void init(const char *name) {
		memset(this->name, 0, 256);
		strncpy(this->name, name, 255);
		this->includeCount = 0;
		this->flag = 0;
	}
	void addInclude(Source *inc) {
		if (this->includeCount < 100) {
			this->includes[this->includeCount++] = inc;
		}
	}
	char* getName() { return this->name;  }
	size_t hasIncludes() { return this->includeCount; }
	Source* getInclude(size_t ix) {
		Source *source = NULL;
		if (ix >= 0 && ix < this->includeCount) {
			source = this->includes[ix];
		}
		return source;
	}
	size_t getFlag() { return this->flag; }
	size_t setFlag(size_t flag) { return this->flag = flag; }
};

char* declaresInclude(const char* line) {
	char *include = NULL;
	//^\s*include\s*(\s*['"]\w+['"]\s*)\s*$
	const char *pos = line;
	// ^\s*
	size_t len = strspn(pos, " \t");
	pos += len;
	//include
	if (strncmp(pos, "include", 7) == 0) {
		// \s*
		pos += 7;
		size_t len = strspn(pos, " \t");
		pos += len;
		// (
		if (*pos == '(') {
			pos++;
			// \s*
			size_t len = strspn(pos, " \t");
			pos += len;
			char quote = *pos;
			// ['"]
			if (quote == '\'' || quote == '\"') {
				pos++;
				const char *start = pos;
				// \w+
				pos = strchr(pos, quote);
				if (pos != NULL) {
					len = pos - start;
					include = new char[len + 1];
					size_t i = 0;
					for (; i < len; i++) {
						include[i] = start[i];
					}
					include[i] = '\0';
				}
			}
		}
	}
	return include;
}

Source* findOrCreateSource(const char *name, Source** sources, size_t& sourceCount) {
	size_t found = -1;
	for (size_t i = 0; i < sourceCount; i++) {
		if (strcmp(sources[i]->getName(), name) == 0) {
			found = i;
			break;
		}
	}
	Source *source = NULL;
	if (found == -1) {
		source = new Source(name);
		sources[sourceCount++] = source;
	}
	else {
		source = sources[found];
	}
	return source;
}

PathInfo *g_inputPathInfo;
Source *g_sources[255];
size_t g_sourceCount = 0;

FILE *g_fout = NULL;

typedef int callback(void*);

Source* traverseDFS(Source *node, callback pre, callback post) {
	node->setFlag(1);
	Source *res = NULL;
	if (!pre(node)) {
		for (size_t i = 0; i < node->hasIncludes(); i++) {
			Source *include = node->getInclude(i);
			if (!include->getFlag()) {
				if ((res = traverseDFS(include, pre, post)) != NULL) {
					break;
				}
			}
		}
		if (res == NULL) {
			if (post(node)) {
				res = node;
			}
		}
	} else {
		res = node;
	}
	return res;
}

int pre(void* node) {
	int res = 0;
	Source *source = (Source*)node;
	// Process file and get includes
	const char *file = source->getName();
	printf("Processing %s\n", file);
	// Try to open file
	FILE *fin = fopen(file, "r");
	if (fin != NULL) {
		// read file content
		char line[1024];
		while (!feof(fin)) {
			if (fgets(line, 1024, fin) != NULL) {
				// check line for having a declaration for an include
				char *include = declaresInclude(line);
				if (include != NULL) {
					// Create full path of include
					PathInfo includePathInfo(include);
					char tmpIncludeName[255] = "";
					if (strlen(includePathInfo.getPath()) == 0 && strlen(g_inputPathInfo->getPath()) > 0) {
						strcpy(tmpIncludeName, g_inputPathInfo->getPath());
						strcat(tmpIncludeName, "\\");
					}
					strcat(tmpIncludeName, include);
					printf(" - includes '%s'\n", include);
					// Add include dependency to source
					source->addInclude(findOrCreateSource(tmpIncludeName, g_sources, g_sourceCount));
				}
			}
		}
		fclose(fin);
	} else {
		printf("Could not open '%s'\n", file);
		res = 1;
	}
	return res;
}

int post(void* node) {
	int res = 0;
	Source *source = (Source*)node;
	// Copy file content except includes to the output
	const char *file = source->getName();
	FILE *fin = fopen(file, "r");
	if (fin != NULL) {
		char line[1024];
		strncpy(line, "/* ***************************************************************************/\n", 255);
		size_t i = 0;
		for (; i < strlen(file); i++) {
			line[3 + i] = file[i];
		}
		line[3 + i] = ' ';
		fputs(line, g_fout);
		// read file content
		while (!feof(fin)) {
			if (fgets(line, 1024, fin) != NULL) {
				// check line for having a declaration for an include
				char *include = declaresInclude(line);
				if (include == NULL) {
					fputs(line, g_fout);
				}
			}
		}
		fputs("\n\n", g_fout);
		fclose(fin);
	} else {
		printf("Could not open '%s'\n", file);
		res = 1;
	}
	printf("%s added\n", file);
	return res;
}

void main(int argc, char **argv) {
	int error = 1;
	if (argc > 1) {
		char *input = _strdup(argv[1]);
		error = 2;
		if (input != NULL) {
			char output[256];
			if (argc > 2) {
				strncpy(output, argv[2], 255);
			}
			else {
				char fileName[256];
				strncpy(fileName, input, 255);
				// "cut off" the extension
				char *pos = strrchr(fileName, '.');
				if (pos != NULL) {
					*pos = '\0';
				}
				size_t len = strlen(fileName);
				if (len > 255 - 9) {
					pos = &fileName[255 - 9];
				}
				len = snprintf(output, 255, "%s.build.js", fileName);
				output[len] = '\0';
			}
			g_inputPathInfo = new PathInfo(input);
			printf("Base dir: %s\n", g_inputPathInfo->getPath());

			char outName[256];
			strncpy(outName, g_inputPathInfo->getPath(), 247);
			strncat(outName, "\\", 248);
			strncat(outName, g_inputPathInfo->getFileName(), 248);
			strncat(outName, ".out.js", 255);
			g_fout = fopen(outName, "w");
			if (g_fout != NULL) {
				printf("Output: %s\n", outName);
				Source *source = findOrCreateSource(input, g_sources, g_sourceCount);
				source = traverseDFS(source, pre, post);
				if (source != NULL) {
					printf("Error at '%s'\n", source->getName());
				}
				error = 0;
				fclose(g_fout);
			} else {
				printf("Could not open '%s'\n", outName);
				error = 1;
			}
		}
	}
	if (error != 0) {
		printf("Error #%d", error);
	}
}