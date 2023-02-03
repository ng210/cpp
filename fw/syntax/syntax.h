#ifndef __SYNTAX_H
#define __SYNTAX_H

#include "collection/graph.h"
#include "collection/parray.h"
#include "syntax/expression.h"
#include "syntax/grammar.h"

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

    class Syntax {
    protected: PROP_R(void*, literal);
    protected: PROP_R(void*, literalType);
    protected: PROP_R(int, literalCode);
    protected: PROP_R(void*, wildcard);
    protected: PROP_R(void*, wildcardType);
    protected: PROP_R(int, wildcardCode);
    protected: PROP_R(char*, startTerm);
    protected: PROP_R(Map, symbols);
    protected: PROP_R(PArray*, prototypes);
    protected: PROP_R(PArray*, rules);
    protected: PROP_R(Grammar*, grammar);

    public:
        Syntax(void* grammar, bool debug);
        virtual ~Syntax();

        Expression* createExpression();
        Expression* parse(char* input, bool ignoreCase);
    };
}

#endif
