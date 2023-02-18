#ifndef __SYNTAX_NODE_H
#define __SYNTAX_NODE_H

#include "collection/tree.h"
#include "collection/map.h"
#include "syntax/grammar-defs.h"

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

    class Node : public Vertex {
    protected: PROP_R(Value, value);            // interpreted value (parsed or calculated)
    protected: PROP_R(char*, term);             // input term associated with the node
    protected: PROP_R(GrammarType*, type);      // type defined in the grammar
    protected: PROP(int, code);                 // code used in the transformed grammatic rules
    public:
        Node(int code, void* type, char* term);
        Array* getValue();
    };
}

#endif