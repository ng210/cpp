#include <stdio.h>
#include <string.h>

#include "consoleapp.h"
#include "source.h"

NS_FW_BASE_USE

typedef int callback(void*);

class JsBuild {
	int error_;
	PathInfo* inputPathInfo_;
	char* basePath_;
	char* input_;
	char* output_;
	bool verbose_;
	Tree* sourceTree_;
	FILE* fout_;
	PArray* out_;

private:
	static char* parseSymbol(char* pos, char* separators) {
		// match /[A-Za-z0-9_]+/
		char ch = 0;
		while ((ch = *pos) != '\0') {
			if (strchr(separators, ch)) break;
			if (ch == '_' || ch >= '0' && ch <= '9' || ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z') {
				pos++;
			} else {
				pos = NULL;
				break;
			}
		}
		return pos;
	}
	
	void addHeader(const char* fileName, PArray* content) {
		char header[] = "/* ***************************************************************************/";
		size_t len = NS_FW_BASE::strlen(fileName);
		memcpy(&header[3], fileName, len);
		header[3 + len] = ' ';
		content->insertAt(0, NS_FW_BASE::strdup((const char*)& header));
	}

	static PArray* declaresPublish(char* line) {
		// match /^\s*publish\s*(\s*<symbol>\s*,\s*<name>\s*,\s*<context>/
		//	symbol: [A-Za-z0-9_]+
		//	name: '[^']+'
		//	context: [A-Za-z0-9_]+
		char* pos = line;
		char* pre = NULL;
		char* symbol = NULL;
		char* name = NULL;
		char* context = NULL;
		PArray* publish = NULL;
		while (true) {
			// match: ^\s*
			pos += strspn(pos, " \t");
			pre = pos;
			// match: publish
			if (NS_FW_BASE::strncmp(pos, "publish", 7) != 0) break;
			pos += 7;
			// match: \s*(\s*
			pos += strspn(pos, " \t");
			if (*pos++ != '(') break;
			pos += strspn(pos, " \t");
			// match: symbol
			symbol = pos;
			if (!(pos = parseSymbol(pos, " \t,"))) break;
			symbol = substr(symbol, 0, pos - symbol);
			// match: \s*,\s*
			pos += strspn(pos, " \t");
			if (*pos++ != ',') break;
			pos += strspn(pos, " \t");
			// match: name
			if (*pos++ != '\'') break;
			name = pos;
			size_t len = strcspn(pos, "'");
			if (len == 0) break;
			pos += len;
			if (*pos++ != '\'') break;
			name = substr(name, 0, len);
			publish = NEW_(PArray, 4);
			pre = substr(line, 0, pre - line);
			publish->add(pre);
			publish->add(symbol);
			publish->add(name);
			// context is optional
			// match: \s*,\s*
			pos += strspn(pos, " \t");
			if (*pos++ != ',') break;
			pos += strspn(pos, " \t");
			// match: context
			context = pos;
			if (!(pos = parseSymbol(pos, "),"))) break;
			context = substr(context, 0, pos - context);
			publish->add(context);
			break;
		}
		return publish;
	}

	char* declaresInclude(char* line) {
		// match: /^\s*include\s*(\s*'<path>'\s*)/
		//  path: [^']
		char* pos = line;
		char* path = NULL;
		while (true) {
			// match: ^\s*
			pos += strspn(pos, " \t");
			// match: include
			if (NS_FW_BASE::strncmp(pos, "include", 7) != 0) break;
			pos += 7;
			// match: \s*(\s*
			pos += strspn(pos, " \t");
			if (*pos++ != '(') break;
			pos += strspn(pos, " \t");
			// match: name
			if (*pos++ != '\'') break;
			char* start = pos;
			size_t len = strcspn(pos, "'");
			if (len == 0) break;
			pos += len;
			if (*pos++ != '\'') break;
			// match: \s*)
			pos += strspn(pos, " \t");
			if (*pos++ != ')') break;
			path = substr(start, 0, len);
			break;
		}
		return path;
	}

	const char* processPublish(PArray* publish) {
		char* pre = (char*)publish->getAt(0);
		char* symbol = (char*)publish->getAt(1);
		char* name = (char*)publish->getAt(2);
		char* context = (char*)publish->getAt(3);
		// context = context || self;
		if (context == NULL) {
			context = "self";
		}
		// mdl.symbols[name] = obj;

		// context[name] = obj;
		const char format[] = "%s%s['%s'] = %s;";	// <context>['<name>'] = <symbol>;
		size_t length = sizeof(format) - 8 + NS_FW_BASE::strlen(pre) + NS_FW_BASE::strlen(context) + NS_FW_BASE::strlen(name) + NS_FW_BASE::strlen(symbol) + 1;
		char* newLine = MALLOC(char, length);
		snprintf(newLine, length - 1, format, pre, context, name, symbol);
		if (this->verbose_) printf(" Added line: >%s<\n", newLine);
		return newLine;
	}

	bool getInclude(char* path, Node*& node) {
		bool isNew = false;
		node = NULL;
		// check include in tree of processed sources
		int ix = 0;
		node = (Node*)sourceTree_->nodes()->binSearch(path, ix, CompareSourceName);
		if (node == NULL) {
			// check file
			if (File::exists(path)) {
				isNew = true;
			}
		}
		return isNew;
	}

	void processInclude(char* path, Node* node) {
		PathInfo includePathInfo(path);
		Node* includeNode = NULL;
		char* includePath = NULL;

		bool isNew = false;
		while (true) {
			// 1st attempt: inputPath
			if (NS_FW_BASE::strlen(this->inputPathInfo_->path()) > 0) {
				char* tmp1 = str_concat(3, this->inputPathInfo_->path(), "\\", path);
				includePath = str_replace(tmp1, "/", "\\");
				FREE(tmp1);
				isNew = getInclude(includePath, includeNode);
				if (isNew || includeNode != NULL) break;
				FREE(includePath);
			}
			// 2nd attempt: basePath
			if (NS_FW_BASE::strlen(this->basePath_) > 0) {
				char* tmp1 = str_concat(3, this->basePath_, "\\", path);
				includePath = str_replace(tmp1, "/", "\\");
				FREE(tmp1);
				isNew = getInclude(includePath, includeNode);
				if (isNew || includeNode != NULL) break;
				FREE(includePath);
			}
			break;
		}

		Source* include = NULL;
		if (isNew) {
			include = this->createSource(includePath);
			this->sourceTree_->addNode(node, include);
		} else if (includeNode != NULL) {
			this->sourceTree_->addEdge(node, includeNode, NULL);
			include = (Source*)includeNode->value();
		}
		if (include != NULL) {
			if (this->verbose_) printf(" - includes '%s'\n", includePath);
			//((Source*)node->value())->addInclude(include);
		} else {
			// include source does not exist
			this->error_ = 3;
		}
		FREE(includePath);
	}

	void processContentLine(Source* source, Node* node, UINT32& lineNumber, char* line) {
		PArray* content = source->content();
		const char* fileName = source->name();
		PArray* publish = declaresPublish(line);
		if (publish != NULL) {
			char* newLine = str_concat("// ", line);
			FREE(line);
			content->setAt(lineNumber, newLine);
			content->insertAt(++lineNumber, (void*)processPublish(publish));
			ARRAY_FOREACH(publish, FREE(value));
			DEL_(publish);
		} else {
			char* includePath = declaresInclude(line);
			if (includePath != NULL) {
				char* newLine = str_concat("// ", line);
				FREE(line);
				content->setAt(lineNumber, newLine);
				processInclude(includePath, node);
				FREE(includePath);
			}
		}
	}

	Source* createSource(char* name) {
		Source* source = NEW_(Source, name);
		if (source->content() == NULL) {
			DEL_(source);
			printf("%s not found!\n", name);
			source = NULL;
		}
		return source;
	}

	static int CompareSourceName(void* item, unsigned int ix, Collection* array, void* key) {
		Node* node = (Node*)item;
		Source* source = (Source*)node->value();
		return strncmp(source->name(), (char*)key);
	}

	static int deleteNodeHandler(void* node, size_t argc, ...) {
		DEL_((Source*)((Node*)node)->value());
		return 0;
	}

	static int pre(void* item, size_t argc, ...) {
		va_list args;
		va_start(args, argc);
		Node* node = (Node*)item;
		JsBuild* app = va_arg(args, JsBuild*);
		Source* source = (Source*)node->value();
		if (app->verbose_) printf("Processing %s\n", source->name());

		// Process file and get includes
		if (source->content() != NULL) {
			app->addHeader(source->name(), source->content());
			ARRAY_FOREACH(source->content(), app->processContentLine(source, node, i, (char*)value));
			//input->cleanUp();
		}

		return 0;
	}

	static int post(void* item, size_t argc, ...) {
		va_list args;
		va_start(args, argc);
		Node* node = (Node*)item;
		JsBuild* app = va_arg(args, JsBuild*);

		//Object* res = NULL;
		Source* source = (Source*)node->value();
		if (source->flag() == 0) {
			const char* file = source->name();
			app->out_->join(source->content());
			if (app->verbose_) printf("Added %s\n", file);
			source->flag(1);
		}
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
			basePath_ = NS_FW_BASE::strdup("");
		}
		else {
			char* tmp = str_replace(basePath_, "/", "\\");
			if (!str_ends(tmp, "\\")) {
				basePath_ = str_concat(tmp, "\\");
				FREE(tmp);
			}
			else {
				basePath_ = tmp;
			}
		}

		if (inputFile == NULL || NS_FW_BASE::strlen(inputFile) == 0) {
			error_ = 2;
		}
		else {
			input_ = NS_FW_BASE::strdup(inputFile);
			inputPathInfo_ = NEW_(PathInfo, input_);

			if (outputFile == NULL || NS_FW_BASE::strlen(outputFile) == 0) {
				char* tmp = str_join(4, "", inputPathInfo_->path(), "\\", inputPathInfo_->fileName(), ".out.js");
				output_ = str_replace(tmp, "/", "\\");
				FREE(tmp);
			}
			else {
				output_ = str_replace(outputFile, "/", "\\");
			}
		}
	}


public:
	JsBuild(void) {
		error_ = 0;
		inputPathInfo_ = NULL;
		basePath_ = NULL;
		input_ = NULL;
		output_ = NULL;
		verbose_ = false;
		sourceTree_ = NEW_(Tree);
		sourceTree_->deleteNodeHandler = deleteNodeHandler;
		fout_ = NULL;
		out_ = NEW_(PArray);
	}
	~JsBuild(void) {
		//ARRAY_FOREACH(sources_->nodes(), DEL_((Source*)value));
		//ARRAY_FOREACH(sources_->edges(), DEL_((Source*)value));
		//sources_->cleanUp();
		DEL_(sourceTree_)
		DEL_(inputPathInfo_);
		//ARRAY_FOREACH(out_, FREE(value));
		DEL_(out_);
		DEL_(basePath_);
		FREE(output_);
		//FREE(input_);
	}

	int main(Map* args) {
		processArguments(args);
		if (error_ == 0) {
			if (verbose_) {
				printf("Base path: %s\n", basePath_);
				printf("Input: %s\n", input_);
				printf("Output: %s\n\n", output_);
			}
			char* buf = NULL;
			Source* source = createSource(input_);
			if (source != NULL) {
				sourceTree_->addNode(NULL, source);
				Node *lastSource = sourceTree_->traverseDFS(JsBuild::pre, NULL, JsBuild::post, this);
				if (lastSource != NULL) {
					printf("Error at '%s'\n", source->name());
					error_ = 1;
				}
				else {
					error_ = 0;
					char* output = out_->str_join("\n");
					size_t len = NS_FW_BASE::strlen(output);
					File::write(output_, (BYTE*)output, len);
					FREE(output);
				}
			}
		}
		FREE(input_);
		return error_;
	}
};

int _main(NS_FW_BASE::Map* args) {
	MemoryMgr::isDebugOn = false;
	JsBuild app;
	int error = app.main(args);
	if (error != 0) {
		printf("Error #%d\n", error);
	}
	return error;
}
