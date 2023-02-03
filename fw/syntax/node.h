#ifndef __SYNTAX_NODE_H
#define __SYNTAX_NODE_H

#include "collection/tree.h"
#include "collection/map.h"

#ifndef NS_FW_SYNTAX
#define NS_FW_SYNTAX syntax
#endif

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

    typedef union Value {
        long i;
        double d;
        void* p;
        char* s;
        Value() : p(0) {}
        Value(int iv) : i(iv) {}
        Value(double dv) : d(dv) {}
        Value(void* pv) : p(pv) {}
    } Value;

    class Node : public Vertex {
    protected: PROP_R(Value, value);    // interpreted value (parsed or calculated)
    protected: PROP_R(char*, term);     // input term associated with the node
    protected: PROP_R(void*, type);     // prototype defined in the grammar
    protected: PROP_R(int, code);       // code used in the transformed grammatic rules
    public:
        Node(int code, void* type, char* term);
        Array* getValue();
    };
}

#endif