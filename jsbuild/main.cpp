#include <stdio.h>
#include <string.h>

#include "consoleapp.h"
#include "utils/PathInfo.h"

NS_FW_BASE_USE

typedef int callback(void*);

class Source : Object {
	String* name_;
	Array includes_;
	size_t flag_;
	Array* content_;
public:
	Source(void) {
		init(NEW_(String));
	}
	Source(String* name) {
		init(name);
	}
	~Source(void) {
		for (size_t i = 0; i < includes_.length(); i++) {
			Source* source = (Source*)includes_[i];
			if (--source->flag_ == 0) {
				DEL_(source);
			}
		}
		if (content_ != NULL) {
			content_->cleanUp();
			DEL_(content_);
		}
		DEL_(name_);
	}
	void init(String* name) {
		name_ = NEW_(String, *name);
		flag_ = 0;
		char* fileName = name_->toString();
		char* buf = File::read(fileName);
		FREE(fileName);
		content_ = NULL;
		if (buf != NULL) {
			String* str = NEW_(String, buf);
			content_ = str->split("\n");
			DEL_(str);
		}
	}
	void addInclude(Source *inc) {
		includes_.push(inc);
	}
	String* getName() { return name_; }
	size_t hasIncludes() { return includes_.length(); }
	Source* getInclude(size_t ix) {
		return (Source*)includes_[ix];
	}
	size_t getFlag() { return flag_; }
	size_t setFlag(size_t flag) { return flag_ = flag; }
	Array* getContent() { return content_; }
};

class JsBuild {
	int error_;
	PathInfo* inputPathInfo_;
	String* basePath_;
	String* input_;
	String* output_;
	bool verbose_;
	Tree* sources_;
	FILE* fout_;
	Array* out_;

public:
	JsBuild(void) {
		error_ = 0;
		sources_ = NEW_(Tree);
		out_ = NEW_(Array);
	}
	~JsBuild(void) {
		sources_->cleanUp();
		DEL_(sources_)
		DEL_(inputPathInfo_);
		DEL_(out_);
		DEL_(basePath_);
	}

	Source* createSource(String* name) {
		Source* source = NEW_(Source, name);
		if (source->getContent() == NULL) {
			DEL_(source);
			char* buf = name->toString();
			printf("%s not found!\n", buf);
			FREE(buf);
			source = NULL;
		}
		return source;
	}

	Node* findSource(String* name) {
		Node* res = NULL;
		for (size_t i = 0; i < sources_->nodes()->length(); i++) {
			Node* node = (Node*)sources_->nodes()->get(i);
			Source* source = (Source*)node->value();
			if (name->compareTo(source->getName()) == 0) {
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

	static Object* pre(size_t count, ...) {
		va_list args;
		va_start(args, count);
		Node* node = va_arg(args, Node*);
		JsBuild* app = va_arg(args, JsBuild*);

		Object* res = NULL;
		Source *source = (Source*)node->value();
		// Process file and get includes
		char* fileName = source->getName()->toString();
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
			sourceContent->unshift(NEW_(String, (const char*)&line));
			//Array newContent;
			for (size_t i = 0; i < sourceContent->length(); i++) {
				String* str = (String*)(*sourceContent)[i];
				char* buf = str->toString();
				const char* exports = declaresExports(buf);
				if (exports != NULL) {
					const char* format = "module['%s']=%s";
					size_t length = 20 + NS_FW_BASE::strlen(exports);
					String* ref1 = NEW_(String, (const char*)&fileName[app->basePath_->length()-1]);
					String* ref2 = ref1->replace("\\", "/");
					DEL_(ref1);
					char* moduleRef = ref2->toString();
					length += ref2->length();
					DEL_(ref2);
					char* newLine = MALLOC(char, length);
					// change into module[<moduleRef>]=<exports>
					snprintf(newLine, length-1, format, moduleRef, exports);
					if (app->verbose_) printf(" export '%s'\n", newLine);
					(*str) = (const char*)newLine;
					FREE(moduleRef);
					FREE(newLine);
				}
				char *includeNameBuf = declaresInclude(buf);
				FREE(buf);
				if (includeNameBuf != NULL) {
					PathInfo includePathInfo(includeNameBuf);
					Array arr(1, NEW_(String, app->basePath_->toString()));
					if (includePathInfo.getPath()->length() == 0 && app->inputPathInfo_->getPath()->length() > 0) {
						arr.push(NEW_(String, *app->inputPathInfo_->getPath()));
						arr.push("\\");
					}
					String* tmp = NEW_(String, includeNameBuf);
					arr.push(tmp->replace("/", "\\"));
					DEL_(tmp);
					// Add include dependency to source
					String* includeName = arr.join("");
					//FREE(includeNameBuf);
					buf = includeName->toString();
					if (app->verbose_) printf(" includes '%s'\n", buf);
					DEL_(buf);
					arr.cleanUp();
					Node* include = app->findSource(includeName);
					if (include == NULL) {
						Source* includeSource = app->createSource(includeName);
						if (includeSource == NULL) {
							app->error_ = 3;
						} else {
							app->sources_->addNode(node, (Object*)includeSource);
						}
					} else {
						app->sources_->addEdge(node, include, NULL);
					}
					DEL_(includeName);
					Array* removed = sourceContent->splice(i, 1);
					DEL_(removed);
					DEL_(str);
					i--;
					//(*str) = "";
				}
			}
			//input->cleanUp();
		}
		FREE(fileName);

		return res;
	}

	static Object* post(size_t count, ...) {
		va_list args;
		va_start(args, count);
		Node* node = va_arg(args, Node*);
		JsBuild* app = va_arg(args, JsBuild*);

		//Object* res = NULL;
		Source *source = (Source*)node->value();
		char *file = source->getName()->toString();
		Array* arr = app->out_->concat(source->getContent());
		DEL_(app->out_);
		app->out_ = arr;
		if (app->verbose_) printf(" added %s\n", file);
		FREE(file);
		return NULL;
	}

	void processArguments(Map* args) {
		String argBase("base");
		String argInput("in");
		String argOutput("out");
		String argVerbose("v");
		basePath_ = (String*)args->get(&argBase);
		String* inputFile = (String*)args->get(&argInput);
		String* outputFile = ((String*)args->get(&argOutput));
		verbose_ = args->get(&argVerbose) != Null;

		if (basePath_ == Null || basePath_->length() == 0) {
			if (verbose_) printf("No base path defined.\n");
			basePath_ = NEW_(String, "");
		} else {
			String* tmp = basePath_->replace("/", "\\");
			if (!tmp->endsWith("\\")) {
				basePath_ = tmp->concat("\\");
				DEL_(tmp);
			} else {
				basePath_ = tmp;
			}
		}
		
		if (inputFile == Null || inputFile->length() == 0) {
			error_ = 2;
		} else {
			if (!inputFile->startsWith(basePath_)) {
				input_ = basePath_->concat(inputFile);
			}
			else {
				input_ = *inputFile;
			}
			inputPathInfo_ = NEW_(PathInfo, input_);
		}

		if (outputFile == Null || outputFile->length() == 0) {
			Array arr;
			arr.push(NEW_(String, *inputPathInfo_->getPath()));
			arr.push(NEW_(String, "\\"));
			arr.push(2, NEW_(String, *inputPathInfo_->getFileName()), NEW_(String, ".out.js"));
			String* tmp = arr.join("");
			output_ = tmp->replace("/", "\\");
			DEL_(tmp);
			arr.cleanUp();
		} else {
			output_ = outputFile->replace("/", "\\");
		}
	}

	int main(Map* args) {
		processArguments(args);
		if (error_ == 0) {
			if (verbose_) {
				char* buf = basePath_->toString();
				printf("Base path: %s\n", buf);
				FREE(buf);
				buf = input_->toString();
				printf("Input: %s\n", buf);
				FREE(buf);
				buf = output_->toString();
				printf("Output: %s\n", buf);
				FREE(buf);
			}
			char* buf = NULL;
			Source *source = createSource(input_);
			if (source != NULL) {
				sources_->addNode(NULL, (Object*)source);
				Node *lastSource = sources_->traverseDFS(JsBuild::pre, NULL, JsBuild::post, (Object*)this);
				if (lastSource != NULL) {
					buf = source->getName()->toString();
					printf("Error at '%s'\n", buf);
					FREE(buf);
					error_ = 1;
				}
				else {
					error_ = 0;
					String* out = out_->join("\n");
					buf = out->toString();
					File::write(output_, buf, out->length());
					DEL_(out);
					FREE(buf);
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
