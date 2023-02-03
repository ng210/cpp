#include "syntax/grammar.h"

using namespace NS_FW_SYNTAX;

#define KEY_SIZE 32

Grammar::Grammar() {
	rules_.init(sizeof(GrammarRule), 32);
	types_.initialize(KEY_SIZE, sizeof(GrammarType), Map::hashingStr, Collection::compareStrReverse);
}
Grammar::~Grammar() {
}

void Grammar::addRule(GrammarRule* rule) {
	// keep rules sorted by priority and input
	var ix = 0;
	var storedRule = rules_.binSearch(rule, ix);
}

void Grammar::addType(Key key, GrammarType* type) {
	char k[KEY_SIZE];
	strncpy(k, 32, (char*)key.p);
	types_.add(&k, type);
}

int Grammar::compareRules(void* item, Key key, UINT32 ix, Collection* collection, void* args) {
	var ruleA = (GrammarRule*)item;
	var ruleB = (GrammarRule*)key.p;
	return ruleA->priority - ruleB->priority;
}