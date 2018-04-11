#include <stdio.h>
#include <string.h>

#include "consoleapp.h"

NS_FW_BASE_USE

typedef int callback(void*);

class Source {
	const char* name_;
	Array includes_;
	size_t flag_;
	Array* content_;
public:
	Source(void) {
		init(NULL);
	}
	Source(const char* name) {
		init(name);
	}
	~Source(void) {
		for (UINT32 i = 0; i < includes_.length(); i++) {
			Source* source = (Source*)includes_.getAt(i);
			if (--source->flag_ == 0) {
				DEL_(source);
			}
		}
		if (content_ != NULL) {
			ARRAY_FOREACH(content_, FREE(value));
			DEL_(content_);
		}
		DEL_(name_);
	}
	void init(const char* name) {
		name_ = name;
		flag_ = 0;
		const char* buf = (const char*)File::read(name_);
		content_ = NULL;
		if (buf != NULL) {
			content_ = Array::str_split(buf, "\n");
		}
	}
	void addInclude(Source *inc) {
		includes_.add(inc);
	}
	const char* getName() { return name_; }
	size_t hasIncludes() { return includes_.length(); }
	Source* getInclude(UINT32 ix) {
		return (Source*)includes_.getAt(ix);
	}
	size_t getFlag() { return flag_; }
	size_t setFlag(size_t flag) { return flag_ = flag; }
	Array* getContent() { return content_; }
};

class JsBuild {
	int error_;
	PathInfo* inputPathInfo_;
	char* basePath_;
	char* input_;
	char* output_;
	bool verbose_;
	Tree* sourceTree_;
	FILE* fout_;
	Array* out_;

public:
	JsBuild(void) {
		error_ = 0;
		sourceTree_ = NEW_(Tree);
		out_ = NEW_(Array);
	}
	~JsBuild(void) {
		//ARRAY_FOREACH(sources_->nodes(), DEL_((Source*)value));
		//ARRAY_FOREACH(sources_->edges(), DEL_((Source*)value));
		//sources_->cleanUp();
		DEL_(sourceTree_)
		DEL_(inputPathInfo_);
		DEL_(out_);
		DEL_(basePath_);
	}

	Source* createSource(char* name) {
		Source* source = NEW_(Source, name);
		if (source->getContent() == NULL) {
			DEL_(source);
			printf("%s not found!\n", name);
			source = NULL;
		}
		return source;
	}

	Node* findSource(char* name) {
		Node* res = NULL;
		for (UINT32 i = 0; i < sourceTree_->nodes()->length(); i++) {
			Node* node = (Node*)sourceTree_->nodes()->getAt(i);
			Source* source = (Source*)node->value();
			if (strncmp(name, source->getName()) == 0) {
				res = node;
				break;
			}
		}
		return res;
	}

	static char* declaresInclude(const char* line) {
		char *include = NULL;
		//^\s*include\s*(\s*['"]\w+['"]\s*)\s*$
		const char *pos = line;
		// ^\s*
		size_t len = strspn(pos, " \t");
		pos += len;
		//include
		if (NS_FW_BASE::strncmp(pos, "include", 7) == 0) {
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
						include = MALLOC(char, len + 1);
						strncpy(include, len, start);
					}
				}
			}
		}
		return include;
	}

	static const char* declaresExports(char* line) {
		const char *res = NULL;
		// if line is ^\s*module.exports\s*=\s*(.+)
		const char *pos = line;
		// ^\s*
		size_t len = strspn(pos, " \t");
		pos += len;
		// module.exports
		if (NS_FW_BASE::strncmp(pos, "module.exports", 14) == 0) {
			pos += 14;
			// \s*
			len = strspn(pos, " \t");
			pos += len;
			// =
			if (*pos == '=') {
				pos++;
				// \s*
				len = strspn(pos, " \t");
				pos += len;
				// (.+) => $1
				res = pos;
				//pos = strchr(pos, ';');
				//*(char*)pos = '\0';
			}
		}
		return res;
	}

	static int pre(void* item, size_t argc, ...) {
		va_list args;
		va_start(args, argc);
		Node* node = va_arg(args, Node*);
		JsBuild* app = va_arg(args, JsBuild*);

		Source *source = (Source*)node->value();
		// Process file and get includes
		const char* fileName = source->getName();
		if (app->verbose_) printf(" processing %s\n", fileName);
		Array* sourceContent = source->getContent();
		if (sourceContent != NULL) {
			char line[] = "/* ***************************************************************************/";
			size_t len = NS_FW_BASE::strlen(fileName);
			memcpy(&line[3], fileName, len);
			//size_t i = 0;
			//for (; i < len; i++) {
			//	line[3 + i] = fileName[i];
			//}
			line[3 + len] = ' ';
			sourceContent->insertAt(0, NS_FW_BASE::strdup((const char*)&line));
			//Array newContent;
			for (UINT32 i = 0; i < sourceContent->length(); i++) {
				char** pLine = (char**)sourceContent->getAt(i);
				const char* exports = declaresExports(*pLine);
				if (exports != NULL) {
					const char* format = "module['%s']=%s";
					size_t length = 20 + NS_FW_BASE::strlen(exports);
					char* moduleRef = str_replace(&fileName[NS_FW_BASE::strlen(app->basePath_) - 1], "\\", "/");
					//String* ref1 = NEW_(String, (const char*)&fileName[app->basePath_->length()-1]);
					//String* ref2 = ref1->replace("\\", "/");
					//DEL_(ref1);
					//char* moduleRef = ref2->toString();
					length += NS_FW_BASE::strlen(moduleRef);
					char* newLine = MALLOC(char, length);
					// change into module[<moduleRef>]=<exports>
					snprintf(newLine, length-1, format, moduleRef, exports);
					FREE(moduleRef);
					if (app->verbose_) printf(" export '%s'\n", newLine);
					(*pLine) = newLine;
					FREE(newLine);
				}
				char *includeNameBuf = declaresInclude(*pLine);
				if (includeNameBuf != NULL) {
					PathInfo includePathInfo(includeNameBuf);
					Array arr(sizeof(char*));
					arr.add(app->basePath_);
					if (NS_FW_BASE::strlen(includePathInfo.path()) == 0 && NS_FW_BASE::strlen(app->inputPathInfo_->path()) > 0) {
						arr.add(app->inputPathInfo_->path());
						arr.add("\\");
					}
					arr.add(str_replace(includeNameBuf, "/", "\\"));
					// Add include dependency to source
					char* includeName = arr.str_join("");
					if (app->verbose_) printf(" includes '%s'\n", includeName);
					ARRAY_FOREACH((&arr), FREE(value));
					Node* include = app->findSource(includeName);
					if (include == NULL) {
						Source* includeSource = app->createSource(includeName);
						if (includeSource == NULL) {
							app->error_ = 3;
						} else {
							app->sourceTree_->addNode(node, includeSource);
						}
					} else {
						app->sourceTree_->addEdge(node, include, NULL);
					}
					DEL_(includeName);
					char* removed = *(char**)sourceContent->getAt(i);
					sourceContent->removeAt(i);
					FREE(removed);
					i--;
					//(*str) = "";
				}
			}
			//input->cleanUp();
		}

		return 0;
	}

	static int post(void* item, size_t argc, ...) {
		va_list args;
		va_start(args, argc);
		Node* node = va_arg(args, Node*);
		JsBuild* app = va_arg(args, JsBuild*);

		//Object* res = NULL;
		Source* source = (Source*)node->value();
		const char *file = source->getName();
		app->out_->join(source->getContent());
		if (app->verbose_) printf(" added %s\n", file);
		return NULL;
	}

	void processArguments(Map* args) {
		char* argBase = "base";
		char* argInput = "in";
		char* argOutput = "out";
		char* argVerbose = "v";
		basePath_ = (char*)args->get(argBase);
		char* inputFile = (char*)args->get(argInput);
		char* outputFile = (char*)args->get(argOutput);
		verbose_ = args->get(argVerbose) != NULL;

		if (basePath_ == NULL || NS_FW_BASE::strlen(basePath_) == 0) {
			if (verbose_) printf("No base path defined.\n");
			basePath_ = "";
		} else {
			char* tmp = str_replace(basePath_, "/", "\\");
			if (!str_ends(tmp, "\\")) {
				basePath_ = str_concat(tmp, "\\");
				DEL_(tmp);
			} else {
				basePath_ = tmp;
			}
		}
		
		if (inputFile == NULL || NS_FW_BASE::strlen(inputFile) == 0) {
			error_ = 2;
		} else {
			if (!str_starts(inputFile, basePath_)) {
				input_ = str_concat(basePath_, inputFile);
			} else {
				input_ = inputFile;
			}
			inputPathInfo_ = NEW_(PathInfo, input_);

			if (outputFile == NULL || NS_FW_BASE::strlen(outputFile) == 0) {
				char* tmp = str_join(4, "", inputPathInfo_->path(), "\\", inputPathInfo_->fileName(), ".out.js");
				output_ = str_replace(tmp, "/", "\\");
				FREE(tmp);
			} else {
				output_ = str_replace(outputFile, "/", "\\");
			}
		}
	}

	int main(Map* args) {
		processArguments(args);
		if (error_ == 0) {
			if (verbose_) {
				printf("Base path: %s\n", basePath_);
				printf("Input: %s\n", input_);
				printf("Output: %s\n", output_);
			}
			char* buf = NULL;
			Source* source = createSource(input_);
			if (source != NULL) {
				sourceTree_->addNode(NULL, source);
				Node *lastSource = sourceTree_->traverseDFS(JsBuild::pre, NULL, JsBuild::post, this);
				if (lastSource != NULL) {
					printf("Error at '%s'\n", source->getName());
					error_ = 1;
				}
				else {
					error_ = 0;
					char* out = out_->str_join("\n");
					size_t len = NS_FW_BASE::strlen(out);
					//size_t length = 1;
					//ARRAY_FOREACH(out_, length += NS_FW_BASE::strlen((char*)value));
					//char* out = MALLOC(char, length);
					//int offset = 0;
					//ARRAY_FOREACH(out_,
					//	int len = NS_FW_BASE::strlen((char*)value); \
					//	strnncpy(out, length, offset, (char*)value, len); \
					//	offset += len
					//);
					File::write(output_, (BYTE*)out, len);
					FREE(out);
				}
			}
		}
		DEL_(output_);
		DEL_(input_);
		return error_;
	}
};

int _main(NS_FW_BASE::Map* args) {
	//MemoryMgr::isDebugOn = true;
	JsBuild app;
	int error = app.main(args);
	if (error != 0) {
		printf("Error #%d\n", error);
	}
	return error;
}
