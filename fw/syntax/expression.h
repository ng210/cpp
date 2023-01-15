#ifndef __EXPRESSION_H
#define __EXPRESSION_H

#include "collection/map.h"
#include "collection/tree.h"
#include "syntax/grammar.h"

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

    class Syntax;
    class Node;

    class Expression {
    protected: PROP_R(Tree*, tree);
    protected: PROP_R(Syntax*, syntax);
    protected: PROP_R(char*, expression);
    protected: PROP_R(Vertex*, lastNode);
    public:
        Expression(Syntax* syntax);
        Expression* resolve(void* context);
        void* evaluate(void* context);
    private:
        Map* createInOutMap(PArray* ruleIn, PArray* ruleOut, Array* missing);
        PArray* mergeNodes(PArray* nodes, Map* inOutMap);
        PArray* shuffleNodes(PArray* nodes, Map* inOutMap);
        void applyRule(Rule* rule, PArray* nodes, int pos, void* context);
        bool matchRule(Rule* rule, PArray* nodes, void* context);
        Node* createNode(int code, void* type, char* term);
    };

}

#endif
