#include <stdio.h>
#include <string.h>

#include "consoleapp.h"
#include "utils/PathInfo.h"

NS_FW_BASE_USE

typedef int callback(void*);

PathInfo *g_inputPathInfo;
Array* g_sources = NULL;
FILE *g_fout = NULL;
Array* g_out = NULL;


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
		content_->cleanUp();
		DEL_(content_);
		DEL_(name_);
	}
	void init(String* name) {
		name_ = NEW_(String, *name);
		flag_ = 0;
		char* fileName = name_->toString();
		char* buf = File::read(fileName);
		FREE(fileName);
		String* str = NEW_(String, buf);
		content_ = str->split("\n");
		DEL_(str);
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
	Array* getContent() { return content_;  }
};

char* declaresInclude(const char* line) {
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

Source* findOrCreateSource(String* name, Array& sources) {
	size_t found = -1;
	for (size_t i = 0; i < sources.length(); i++) {
		Source* source = (Source*)sources[i];
		if (name->compareTo(source->getName()) == 0) {
			found = i;
			break;
		}
	}
	Source *source = NULL;
	if (found == -1) {
		source = NEW_(Source, name);
		sources.push((Object*)source);
	}
	else {
		source = (Source*)sources[found];
	}
	return source;
}

Source* traverseDFS(Source *node, callback pre, callback post) {
	node->setFlag(node->getFlag() + 1);
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
	char* fileName = source->getName()->toString();
	printf("Processing %s\n", fileName);
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
			char *include = declaresInclude(buf);
			if (include != NULL) {
				PathInfo includePathInfo(include);
				Array arr;
				if (includePathInfo.getPath()->length() == 0 && g_inputPathInfo->getPath()->length() > 0) {
					arr.push(g_inputPathInfo->getPath());
					arr.push('\\');
				}
				arr.push(NEW_(String, include));
				printf(" - includes '%s'\n", include);

				// Add include dependency to source
				String* tmpIncludeName = arr.join("");
				arr.cleanUp();
				source->addInclude(findOrCreateSource(tmpIncludeName, *g_sources));

				DEL_(tmpIncludeName);
				(*str) = "";
			}
			FREE(buf);
		}
		//input->cleanUp();
	} else {
		printf("Could not open '%s'\n", fileName);
		res = 1;
	}
	FREE(fileName);
	return res;
}

int post(void* node) {
	int res = 0;
	Source *source = (Source*)node;
	//// Copy file content except includes to the output
	char *file = source->getName()->toString();
	Array* arr = g_out->concat(source->getContent());
	DEL_(g_out);
	g_out = arr;
	//size_t len = source->getName().length();
	////FILE *fin = fopen(file, "r");
	////if (fin != NULL) {
	//	char line[1024];
	//	strncpy(line, "/* ***************************************************************************/\n", 255);
	//	size_t i = 0;
	//	for (; i < len; i++) {
	//		line[3 + i] = file[i];
	//	}
	//	line[3 + i] = ' ';
	//	g_out.push(line);
	//	//fputs(line, g_fout);
	//	// read file content
	//	while (!feof(fin)) {
	//		if (fgets(line, 1024, fin) != NULL) {
	//			// check line for having a declaration for an include
	//			char *include = declaresInclude(line);
	//			if (include == NULL) {
	//				fputs(line, g_fout);
	//			}
	//		}
	//	}
	//	fputs("\n\n", g_fout);
	//	fclose(fin);
	//} else {
	//	printf("Could not open '%s'\n", file);
	//	res = 1;
	//}
	printf("%s added\n", file);
	FREE(file);
	//DEL_(source);
	return res;
}

int _main(NS_FW_BASE::Array* args) {
	int error = 1;
	g_sources = NEW_(Array);
	g_out = NEW_(Array);
	//MemoryMgr::isDebugOn = true;
	char* buf = NULL;
	size_t argc = args->length();
	if (argc > 1) {
		String *input = (String*)(*args)[1];
		error = 2;
		if (input != NULL) {
			g_inputPathInfo = NEW_(PathInfo, input);
			buf = g_inputPathInfo->getPath()->toString();
			printf("Base dir: %s\n", buf);
			FREE(buf);
			char* output;
			if (argc > 2) {
				output = (*args)[1]->toString();
			}
			else {
				Array arr;
				if (g_inputPathInfo->getPath()->length() > 0) {
					arr.push(NEW_(String, *g_inputPathInfo->getPath()));
					arr.push(NEW_(String, "\\"));
				}
				arr.push(2, NEW_(String, *g_inputPathInfo->getFileName()), NEW_(String, ".out.js"));
				String* str = arr.join("");
				output = str->toString();
				DEL_(str);
				arr.cleanUp();
			}
			printf("Output: %s\n", output);
			Source *source = findOrCreateSource(input, *g_sources);
			Source *lastSource = traverseDFS(source, pre, post);
			if (lastSource != NULL) {
				buf = source->getName()->toString();
				printf("Error at '%s'\n", buf);
				FREE(buf);
				error = 1;
			} else {
				error = 0;
				String* out = g_out->join("\n");
				buf = out->toString();
				File::write(output, buf, out->length());
				DEL_(out);
				FREE(buf);
			}
			DEL_(source);
			DEL_(output);
			DEL_(g_inputPathInfo);
		}
	}
	if (error != 0) {
		printf("Error #%d\n", error);
	}
	DEL_(g_sources);
	DEL_(g_out);
	return error;
}
