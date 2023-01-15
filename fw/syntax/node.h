#ifndef __EXPRESSION_H
#define __EXPRESSION_H

#include "collection/tree.h"
#include "collection/map.h"

NS_FW_BASE_USE

namespace NS_FW_SYNTAX {

    class Node : public Vertex {
    protected: PROP_R(void*, value);    // interpreted value (parsed or calculated)
    protected: PROP_R(char*, term);     // input term associated with the node
    protected: PROP_R(void*, type);     // prototype defined in the grammar
    protected: PROP_R(int, code);       // code used in the transformed grammatic rules
    public:
        Node(int code, void* type, char* term);
        PArray* getValue();
    };
}

#endif