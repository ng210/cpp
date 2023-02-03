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
	char* symbols[] = {
		" ",
		"+", "-", "*", "/",
		"pow", "sqrt", "get",
		"power",
		"(", ")", ","
	};
	GrammarType types[] = {
		// separator
		{  0, NULL, NULL },
		// operations: + - * /
		{  1, "A", false, doOperation2 },
		{  2, "S", false, doOperation2 },
		{  3, "M", false, doOperation2 },
		{  4, "D", false, doOperation2 },
		// functions: pow sqrt get
		{  5, "F", false, doOperation2 },
		{  6, "F", false, doOperation1 },
		{  7, "F", false, doOperation1 },
		// misc: ( ) ,
		{  8, "B1", true, NULL },
		{  9, "B2", true, NULL },
		{ 10, "C",  true, NULL }
	};

	for (var i = 0; i < arraysize(symbols); i++) {
		grammar_->addType(symbols[i], &types[i]);
	}
	grammar_->types().sort();
	
#pragma endregion

#pragma region CreateRules
	//// create rules
	//GrammarRule rule;
	//grammar_->addRule(&rule);
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