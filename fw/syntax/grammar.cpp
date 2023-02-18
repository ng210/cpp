#include "base/memory.h"
#include "syntax/grammar.h"

using namespace NS_FW_SYNTAX;

Grammar::Grammar() {
	initialize();
}

Grammar::~Grammar() {
}

void Grammar::initialize() {
	symbols_.init(SYMBOL_MAX_LENGTH, 32);
	symbols_.compare(Collection::compareStr);
	types_.init(sizeof(GrammarType), 16);
	types_.compare(Grammar::compareTypeByTerm);
	rules_.init(sizeof(GrammarRule), 32);
	rules_.compare(Grammar::compareRules);
	// add the literal symbol for non-keyword terms
	literalType_ = { "[L]", "L", false, NULL};
	literal_ = literalType_.symbol;
	types_.push(&literalType_);
	// add the wildcard symbol
	wildCardType_ = { "[*]", "*", false, NULL};
	wildCard_ = wildCardType_.symbol;
	types_.push(&wildCardType_);
	startTerm_ = NULL;
}

void Grammar::addType(GrammarType* type) {
	Key ix = -1;
	if (types_.binSearch(type, ix) == NULL) {
		types_.insert(ix, type);
	}

	if (symbols_.binSearch(type->symbol, ix) == NULL) {
		symbols_.insert(ix, type->symbol);
	}
}

void Grammar::addRule(GrammarRule* rule) {
	Key ix = 0;
	memset(&rule->inNodes, 0, 2 * sizeof(NodeArray));
	var storedRule = rules_.binSearch(rule, ix);
	if (storedRule == NULL) {
		rules_.insert(ix, rule);
		var symbol = symbols_.binSearch(rule->output, ix);
		if (symbol == NULL) {
			symbols_.insert(ix, rule->output);
			GrammarType type;
			type.term[0] = '\0';
			fmw::strncpy(type.symbol, SYMBOL_MAX_LENGTH, rule->output);
			type.isIgnored = false;
			type.action = NULL;
			var storedType = types_.binSearch(&type, ix, &Grammar::compareTypeBySymbol);
			if (storedType == NULL) {
				types_.insert(ix, &type);
			}
		}
	}
}

void Grammar::symbolsToCodes(char* input, NodeArray* nodes) {
	Key ix = -1;
	var tokens = PArray::str_split(input, " ");
	for (var ti = 0; ti < tokens->length(); ti++) {
		var token = (char*)tokens->get(ti);
		var symbol = symbols_.binSearch(token, ix);
			//[](void* value, Key key, UINT32 ix, Collection* collection, void* args) {
			//	var item = (GrammarType*)value;
			//	return fmw::strncmp(item->symbol, (char*)key.p, SYMBOL_MAX_LENGTH);
			//});
		if (symbol != NULL) {
			nodes->codes[nodes->length] = ix.i;
			nodes->length++;
		}
		else {
			// ERROR: invalid input symbol
		}
	}
	ARRAY_FOREACH(tokens, FREE(value));
	DEL_(tokens);
}

void Grammar::updateTables() {
	// update type codes
	var ix = -1;
	for (var ti = 0; ti < types_.length(); ti++) {
		var type = (GrammarType*)types_.get(ti);
		//symbols_.binSearch(type->symbol, ix);
		((GrammarType*)types_.get(ti))->code = ti;
	}

	// fill rule inCodes/outCode
	for (var ri=0; ri<rules_.length(); ri++) {
		var rule = (GrammarRule*)rules_.get(ri);
		symbolsToCodes(rule->input, &rule->inNodes);
		symbolsToCodes(rule->output, &rule->outNodes);
	}
}

int Grammar::compareTypeBySymbol(COLLECTION_ARGUMENTS) {
	var typeA = (GrammarType*)value;
	var typeB = (GrammarType*)key.p;
	return fmw::strncmp(typeA->symbol, typeB->symbol, SYMBOL_MAX_LENGTH);
}

int Grammar::compareTypeByTerm(COLLECTION_ARGUMENTS) {
	var typeA = (GrammarType*)value;
	var typeB = (GrammarType*)key.p;
	return fmw::strncmp(typeA->term, typeB->term, 16);
}

int Grammar::compareRules(COLLECTION_ARGUMENTS) {
	var ruleA = (GrammarRule*)value;
	var ruleB = (GrammarRule*)key.p;
	return 1000 * (ruleB->priority - ruleA->priority) + ruleA->inNodes.length - ruleB->inNodes.length;
}
