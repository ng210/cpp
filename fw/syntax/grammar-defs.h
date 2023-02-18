#ifndef __SYNTAX_GRAMMAR_DEF_H
#define __SYNTAX_GRAMMAR_DEF_H

#define SYMBOL_MAX_LENGTH 8

#ifndef NS_FW_SYNTAX
#define NS_FW_SYNTAX syntax
#endif

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

	typedef union Value {
		long i;
		double d;
		void* p;
		char* s;
		Value() : p(0) {}
		Value(int iv) : i(iv) {}
		Value(double dv) : d(dv) {}
		Value(void* pv) : p(pv) {}

		bool operator==(int iv) { return this->i == iv;  }
		bool operator==(double dv) { return this->d ==dv; }
		bool operator==(char* sv) { return fmw::strncmp(s, sv) == 0; }
		bool operator==(void* pv) { return p == pv; }
		bool operator!=(int iv) { return this->i != iv; }
		bool operator!=(double dv) { return this->d != dv; }
		bool operator!=(char* sv) { return fmw::strncmp(s, sv) != 0; }
		bool operator!=(void* pv) { return p != pv; }

	} Value;

	class Node;
	typedef Value (GRAMMARACTION)(void* context, Node** inNodes);

	typedef struct GrammarType {
		char term[16];
		char symbol[SYMBOL_MAX_LENGTH];
		bool isIgnored;
		bool isStart;
		GRAMMARACTION* action;
		int code;
	} GrammarType;

	typedef struct NodeArray {
		int length;
		Node* nodes[32];
		int codes[32];
	} NodeArray;

	typedef struct GrammarRule {
		char input[8 * SYMBOL_MAX_LENGTH];
		char output[4 * SYMBOL_MAX_LENGTH];
		int priority;
		GRAMMARACTION* action;
		NodeArray inNodes;
		NodeArray outNodes;
	} GrammarRule;
}

#endif