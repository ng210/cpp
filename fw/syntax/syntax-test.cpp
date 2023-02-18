#include "syntax/syntax-test.h"
#include <math.h>

NS_FW_BASE_USE
using namespace NS_FW_SYNTAX;

SyntaxTest::SyntaxTest() {
	grammar_ = NULL;
	syntax_ = NULL;
}

SyntaxTest::~SyntaxTest() {

}

Value getValue(void* obj, char* field) {
	return 0;
}

Value doOperation1(void* context, Node** nodes) {
	var op = nodes[0];
	var x = nodes[1]->value();
	Value result;
	switch (op->code()) {
		case 6: result = sqrt(x.d); break;
		case 7: result = getValue(context, x.s); break;
	}

	return result;
}

Value doOperation2(void* context, Node** nodes) {
	var op = nodes[0];
	var x = nodes[1]->value(), y = nodes[2]->value();
	Value result;

	switch (op->code()) {
		case 1: result.d = x.d + y.d; break;
		case 2: result.d = x.d - y.d; break;
		case 3: result.d = x.d * y.d; break;
		case 4: result.d = x.d / y.d; break;
		case 5: result.d = pow(x.d, y.d); break;
	}
	
	return result;
}

void SyntaxTest::buildBasicCalculusGrammar() {
	grammar_ = NEW_(Grammar);

#pragma region CreateTypes
	GrammarType types[] = {
		// separator
		{ " ",		"",		 false, false, NULL },				// " "
		// operations: + - * /
		{ "+",		"A",	 false, false, doOperation2 },		// "+"
		{ "-",		"S",	 false, false, doOperation2 },		// "-"
		{ "*",		"M",	 false, false, doOperation2 },		// "*"
		{ "/",		"D",	 false, false, doOperation2 },		// "/"
		// functions: pow sqrt get
		{ "pow",	"F",	 false, false, doOperation2 },		// "pow"
		{ "sqrt",	"F",	 false, false, doOperation1 },		// "sqrt"
		{ "get",	"F",	 false, false, doOperation1 },		// "get"
		// misc: ( ) ,
		{ "(",		"B1",	 false, true, NULL },				// "("
		{ ")",		"B2",	 false, true, NULL },				// ")"
		{ ",",		"C",	 false, true, NULL }				// ","
	};

	// add types
	for (var i = 0; i < arraysize(types); i++) {
		grammar_->addType(&types[i]);
	}
#pragma endregion

#pragma region CreateRules
	GrammarRule rules[] = {
		{ "F B1",		"F1", 100, NULL },
		{ "L",			"",    10, NULL },
		{ "L M",		"M1",  70, NULL },
		{ "L A",		"A1",  50, NULL },
		{ "M1 L",		"L",   80, NULL },
		{ "A1 L",		"L",   60, NULL },
		{ "F1 L",		"F2",  46, NULL },
		{ "F2 B2",		"L",   30, NULL },
		{ "B1 L B2",	"L",   90, NULL },
		{ "F2 C",		"F1",  30, NULL }
	};
	
	for (var i = 0; i < arraysize(rules); i++) {
		grammar_->addRule(&rules[i]);
	}

	grammar_->updateTables();

	printf("\n\nSymbols ############\n");
	grammar_->symbols().apply(
		[](COLLECTION_ARGUMENTS) {
			printf("%2d: '%s'\n", key.i, (char*)value);
			return value;
		}
	);

	printf("\nTypes ############\n");
	grammar_->types().apply(
		[](COLLECTION_ARGUMENTS) {
			var type = (GrammarType*)value;
			printf("%2d: '%s'...'%s'(%d)\n", key.i, type->term, type->symbol, type->code);
			return value;
		}
	);

	printf("\nRules ############\n");
	for (var ri = 0; ri < grammar_->rules().length(); ri++) {
		var rule = (GrammarRule*)grammar_->rules().get(ri);
		printf("%3d: ", rule->priority);
		for (var ci = 0; ci < rule->inNodes.length; ci++) {
			var inCode = rule->inNodes.codes[ci];
			printf("%s(%d)", (char*)grammar_->symbols().get(inCode), inCode);
		}
		printf(" => ");
		for (var ci = 0; ci < rule->outNodes.length; ci++) {
			var outCode = rule->outNodes.codes[ci];
			printf("%s(%d)", (char*)grammar_->symbols().get(outCode), outCode);
		}
		printf("\n");
	}


#pragma endregion
}

void SyntaxTest::testBuildGrammar() {
	buildBasicCalculusGrammar();

	DEL_(grammar_);
}

void SyntaxTest::runAll(int& totalPassed, int& totalFailed) {
	test("Grammar build test", (TestMethod)&SyntaxTest::testBuildGrammar);
	totalPassed += passed_;
	totalFailed += failed_;
}