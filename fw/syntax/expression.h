#ifndef __SYNTAX_EXPRESSION_H
#define __SYNTAX_EXPRESSION_H

#include "collection/map.h"
#include "collection/tree.h"
#include "syntax/grammar.h"
#include "syntax/node.h"

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

    class Syntax;
    class Node;

    class Expression {
    protected: PROP_R(Tree*, tree);
    protected: PROP_R(Syntax*, syntax);
    protected: PROP_R(char*, expression);
    protected: PROP_R(Node*, lastNode);
    public:
        Expression(Syntax* syntax);
        Expression* resolve(void* context);
        void* evaluate(void* context);
    private:
        Map* createInOutMap(PArray* ruleIn, PArray* ruleOut, Array* missing);
        PArray* mergeNodes(PArray* nodes, Map* inOutMap);
        PArray* shuffleNodes(PArray* nodes, Map* inOutMap);
        void applyRule(GrammarRule* rule, PArray* nodes, int pos, void* context);
        bool matchRule(GrammarRule* rule, PArray* nodes, void* context);
        Node* createNode(int code, void* type, char* term);

        char* nodeToString(Node* node, bool simple);
    };

}

#endif
