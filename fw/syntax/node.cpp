#include "base/memory.h"
#include "syntax/node.h"

using namespace NS_FW_SYNTAX;

PArray* Node::getValue() {
    PArray* value = NULL;
    if (edges_.length() > 0) {
        value = NEW_(PArray, edges_.length());
        value_ != NULL ? value->add(value_) : value->add(term_);
        for (var i = 0; i < edges_.length(); i++) {
            var node = (Node*)((Edge*)edges_.get(i))->to();
            node->value_ != NULL ? value->add(node->value_) : value->add(node->term_);
        }
    }
    else {
        value = NEW_(PArray);
        value_ != NULL ? value->add(value_) : value->add(term_);
    }
    return value;
};
