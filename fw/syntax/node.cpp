#include "base/memory.h"
#include "syntax/node.h"

using namespace NS_FW_SYNTAX;

Array* Node::getValue() {
    Array* value = NULL;
    if (edges_.length() > 0) {
        value = NEW_(Array, edges_.length());
        value_.p != NULL ? value->push(value_.p) : value->push(term_);
        for (var i = 0; i < edges_.length(); i++) {
            var node = (Node*)((Edge*)edges_.get(i))->to();
            node->value_.p != NULL ? value->push(node->value_.p) : value->add(node->term_);
        }
    }
    else {
        value = NEW_(Array);
        value_.p != NULL ? value->push(value_.p) : value->push(term_);
    }
    return value;
};
