#ifndef __SYNTAX_GRAMMAR_H
#define __SYNTAX_GRAMMAR_H

#include "collection/array.h"
#include "collection/parray.h"
#include "collection/map.h"
#include "syntax/node.h"

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

	class Grammar {
		static COLLECTION_COMPARE compareTypeBySymbol;
		static COLLECTION_COMPARE compareTypeByTerm;
		static COLLECTION_COMPARE compareRules;
		void symbolsToCodes(char* input, NodeArray* nodes);
	protected: PROP_R(Array, symbols);
	protected: PROP_R(Array, types);
	protected: PROP_R(Array, rules);
	protected: void initialize();
	protected: PROP_R(char*, literal);
	protected: PROP_R(GrammarType, literalType);
	protected: PROP_R(char*, wildCard);
	protected: PROP_R(GrammarType, wildCardType);
	protected: PROP(char*, startTerm);

	public:
		Grammar();
		virtual ~Grammar();

		void addRule(GrammarRule* rule);
		void addType(GrammarType* rule);
		void updateTables();
	};
}

#endif