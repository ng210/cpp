#ifndef __SYNTAX_GRAMMAR_H
#define __SYNTAX_GRAMMAR_H

#include "collection/array.h"
#include "collection/parray.h"
#include "collection/map.h"
#include "syntax/node.h"

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

	typedef Value (GRAMMARACTION)(void* context, Node** inNodes);

	typedef struct GrammarRule {
		char input[256];
		char output[256];
		int priority;
		GRAMMARACTION action;

		PArray inNodes;
		Array inCodes;
		Node* outNode;
		int outCode;
	} GrammarRule;

	typedef struct GrammarType {
		int code;
		char symbol[16];		// symbol used in rules
		bool isIgnored;
		GRAMMARACTION* action;
		//char term[64];		// input term mapped onto this prototype
	} GrammarType;

	class Grammar {
		int compareRules(void* item, Key key, UINT32 ix, Collection* collection, void* args);
	protected: PROP_R(Array, rules);
	protected: PROP_R(Map, types);
	public:
		Grammar();
		virtual ~Grammar();

		void addRule(GrammarRule* rule);
		void addType(Key key, GrammarType* rule);
	};
}

#endif