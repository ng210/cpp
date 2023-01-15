#ifndef __GRAMMAR_H
#define __GRAMMAR_H

#include "collection/array.h"
#include "collection/parray.h"

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

	typedef PArray* (RULEACTION)(void* context, PArray* inNodes);

	class Node;

	typedef struct Rule {
		PArray inNodes;
		Array inCodes;
		Node* outNode;
		int outCode;
		int priority;
		char input[256];
		char output[256];
		RULEACTION action;
	} Rule;

	typedef struct Prototype {
		int code;
		char symbol[16];	// symbol used in rules
		RULEACTION action;	// executed 
		char term[64];		// input string mapped onto this prototype

	} Prototype;
}

#endif