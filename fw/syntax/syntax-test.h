#ifndef __SYNTAX_TEST_H
#define __SYNTAX_TEST_H

#include "test.h"
#include "syntax/grammar.h"
#include "syntax/syntax.h"

NS_FW_BASE_USE
using namespace NS_FW_SYNTAX;

class SyntaxTest : public Test {
	Grammar* grammar_;
	Syntax* syntax_;

	void buildBasicCalculusGrammar();
public:
	SyntaxTest();
	~SyntaxTest();

	void testBuildGrammar();
	//void testBuildSyntax();
	//void testBuildExpression();

	void runAll(int& totalPassed, int& totalFailed);
};

#endif
