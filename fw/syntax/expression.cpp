#include <stdarg.h>
#include "base/memory.h"
#include "syntax/expression.h"
#include "syntax/syntax.h"

NS_FW_BASE_USE
using namespace NS_FW_SYNTAX;

Expression::Expression(Syntax* syntax) {
    syntax_ = syntax;
    expression_ = NULL;
    lastNode_ = NULL;
    tree_ = NULL;
}

Expression* Expression::resolve(void* context) {
    lastNode_ = NULL;
    var len = tree_->vertices().length();
    var nodes = MALLOC(Node*, len);
    for (var i = 0; i < len; i++) {
        nodes[i] = (Node*)tree_->vertices().get(i);
    }
    // try to apply rules as long as there are nodes
    while (len > 0) {
        var r = 0;
        while (r < syntax_->rules()->length()) {
            var rule = (GrammarRule*)syntax_->rules()->get(r);
            if (matchRule(rule, len, nodes, context)) {
                // has a match, start over
                // TODO: sort rules by prio and input length
                r = 0;
            }
            else {
                // has no match, try the next rule
                r++;
            }
        }
        break;
    }
    return this;
}
void* Expression::evaluate(void* context) {
    tree_->DFS(lastNode_, NULL,
        [](Graph* graph, Vertex* vertex, va_list args) {
            var obj = va_arg(args, void*);
            var node = (Node*)vertex;
            if (node->type()->action != NULL) {
                // node handler calls action with the input nodes as arguments
                Node* nodes[16];
                // action returns a (calculated) value of the node
                nodes[0] = node;
                for (var i = 0; i < node->edges().length(); i++) {
                    var edge = (Edge*)node->edges().get(i);
                    nodes[i+1] = (Node*)edge->to();
                }
                node->type()->action(obj, nodes);
            }
            return 1;
        }, NULL, context);
    return lastNode_->data();  //.value;
}
Map* Expression::createInOutMap(NodeArray* inNodes, int outCodesLength, int* outCodes, NodeArray* missing) {
// inOutMap[i] = j, if inNodes[i] == outNodes[j]
// missing[k] = i, i !E outNodes
    var inOutMap = NEW_(Map, sizeof(int), sizeof(int), Map::hashingInt, Map::compareInt);
    var arr = MALLOC(int, outCodesLength);
    memcpy(arr, outCodes, outCodesLength);
    int k = 0;
    for (var i = 0; i < inNodes->length; i++) {
        var found = false;
        var inCode = inNodes->codes[i];
        for (var j = 0; j < outCodesLength; j++) {
            var outCode = arr[j];
            if (inCode == outCode && outCode != syntax_->literalCode()) {
                inOutMap->add(i, &j);
                arr[j] = -1;
                found = true;
                break;
            }
        }
        if (!found) {
            missing->codes[k++] = i;
        }
    }
    missing->length = k;
    FREE(arr);
    return inOutMap;
}
NodeArray* Expression::mergeNodes(NodeArray* input, Map* inOutMap) {
    //if (this.syntax.debug > 2) console.log('merge in: ' + nodes.map(n = > `{${ this.nodeToString(n) } }`).join('  '));
    var len = input->length;
    int parents[32];
    memset(parents, 0, len * sizeof(int));
    for (var i = 0; i < len; i++) {
        var node = (Node*)input->nodes[i];
        // candidate for parent:
        // - appears on both sides of the rule, strongest rule
        if (inOutMap->containsKey(i)) parents[i] += 10;
        // - has an edge
        if (node->edges().length() > 0) parents[i]++;
        // - has an action
        if (node->type()->action != NULL) parents[i]++;
        // - non-literal node
        if (node->code() == syntax_->literalCode()) parents[i] -= 8;
        // - is a start node
        if (node->type()->isStart) parents[i]++;
        // - not ignored
        if (node->type()->isIgnored) parents[i] -= 10;
    }
    // - get the highest rating from the left
    var pi = 0;
    for (var i = 1; i < len; i++) {
        if (parents[i] > parents[pi]) pi = i;
    }

    var parent = (Node*)input->nodes[pi];
    var remains = NEW_(NodeArray);
    var ci = len - inOutMap->size();
    var j = 0;
    for (var i = 0; i < len; i++) {
        var ni = (Node*)input->nodes[i];
        if (i == pi || inOutMap->containsKey(i)) {
            remains->nodes[j] = ni;
            continue;
        }
        else if (!ni->type()->isIgnored) {
            tree_->addEdge(parent, ni, NULL);
            input->nodes[i]= NULL;
        }
        if (ci-- == 0) break;
    }
    //if (this.syntax.debug > 2) console.log('merge out: ' + remains.map(n = > `{${ this.nodeToString(n) }}`).join('  '));
    return remains;
}
NodeArray* Expression::shuffleNodes(NodeArray* input, Map* inOutMap) {
    var arr = MALLOC(NodeArray, 1);
    memcpy(arr->nodes, input->nodes, sizeof(NodeArray));
    for (var j = 0; j < input->length; j++) {
        var ix = inOutMap->containsKey(j) ? *(int*)inOutMap->get(j) : j;
        input->nodes[ix] = arr->nodes[j];
    }
    FREE(arr);
    return input;
}
void Expression::applyRule(GrammarRule* rule, NodeArray* input, int pos, void* context) {
    // match found, replace input by output
    //if (this.syntax.debug > 1) console.log(`match: ${ rs(rule) }`);
    // extract rule's input
    var inNodes = MALLOC(NodeArray, 1);
    inNodes->length = rule->inNodes.length;
    for (var i = 0; i < inNodes->length; i++) {
        inNodes->nodes[i] = input->nodes[pos + i];
        //inNodes->codes[i] = input->codes[pos + i];
    }

    int ruleOutCodesLength = 0;
    int ruleOutCodes[32]; // &rule->outCodes;
    if (rule->action != NULL) {
        var outNodes = (NodeArray*)rule->action(context, inNodes->nodes).p;
        if (outNodes != NULL) {
            for (var j = 0; j < outNodes->length; j++) {
                ruleOutCodes[j++] = outNodes->nodes[j]->code();
                //ruleOut->codes[j] = outNodes->codes[j];
            }
            ruleOutCodesLength = outNodes->length;
        }
    }
    if (rule->inNodes.length < ruleOutCodesLength) {
        // ERROR: "Invalid rule: output cannot be longer than input!"
        return;
    }
    NodeArray* outNodes = NULL;
    // check first output symbol is not empty string -> nodes are replaced
    var code = ruleOutCodes[0];
    var symbol = (char*)syntax_->symbols().get(code);
    if (symbol[0] != '\0') {
        NodeArray missing;
        outNodes = inNodes;
        // get mapping between in-out symbols and missing symbols
        var inOutMap = createInOutMap(&rule->inNodes, ruleOutCodesLength, ruleOutCodes, &missing);
        if (rule->inNodes.length > ruleOutCodesLength) {
            // merge nodes
            outNodes = mergeNodes(inNodes, inOutMap);
        }
    
        // shuffle nodes
        if (inOutMap->size() > 0) {
            var nodes = shuffleNodes(outNodes, inOutMap);
        }
    
        // relabel nodes
        if (outNodes->length > 0) {
            for (var i = 0; i < ruleOutCodesLength; i++) {
                var code = ruleOutCodes[i];
                outNodes->nodes[i]->code(code);
                //outNodes[i].data.type = this.syntax.symbols.getAt(code);
            }
        }
    }
    if (outNodes != NULL) {
        // insert processed nodes
        // nodes[0..n],outNodes,nodes[n..]
        var arr = MALLOC(NodeArray, 1);
        var ptr = &arr->nodes;
        int count = pos;
        memcpy(ptr, input->nodes, count);
        ptr += count;
        count = outNodes->length;
        memcpy(ptr, outNodes, count);
        ptr += count;
        count = input->length - pos;
        memcpy(ptr, &input->nodes[pos], count);
    }

    var node = input->nodes[0];
    if (node != NULL) {
        lastNode_ = node;
    }
    
    //if (this.syntax.debug > 1) console.log(`result: ${nodes.map(x => `${this.syntax.symbols.getAt(x.data.code).symbol}['${x.data.term.replace(/[\n\r]/g, '\\n')}']`)}`);
    //if (this.syntax.debug > 0) console.log('result: ' + nodes.map(n = > `{${ this.nodeToString(n) }}`).join('  '));
}
bool Expression::matchRule(GrammarRule* rule, int& len, Node** nodes, void* context) {
    //        // apply the rule on the input as many times as possible
    //        var hasMatch = false;
    //        for (var n = 0; n < nodes.length;) {
    //            var i = 0;
    //            // rule's input is shorter than the rest of the input
    //            if (rule.in.length <= nodes.length - n) {
    //                while (i < rule.in.length) {
    //                    if (rule.in[i] != nodes[n + i].data.code && rule.in[i] != this.syntax.wildcardCode) break;
    //                    i++;
    //                }
    //                if (i == rule.in.length) {
    //                    // apply the matching rule
    //                    this.applyRule(rule, nodes, n, context);
    //                    hasMatch = true;
    //                    break;
    //                }
    //                else {
    //                    // step to next symbol of input
    //                    n++;
    //                }
    //            }
    //            else {
    //                // rule's input too long, skip to next rule
    //                break;
    //            }
    //        }
    //        return hasMatch;
    return true;
}
Node* Expression::createNode(int code, void* type, char* term) {
    var node = NEW_(Node, code, syntax_->grammar()->types().get(type), term);
    return node;
}
char* Expression::nodeToString(Node* node, bool simple) {
    //var value = typeof node.data.value != = 'object' ? node.data.value.toString() : node.data.term;
    //value = value.replace(/ [\n\r] / g, '\\n');
    //var text = `#${ node.id }:'${value}'(${ this.syntax.symbols.getAt(node.data.code).symbol }:${ node.data.type.symbol })`;
    //if (!simple) {
    //    text += `${node.edges.map(x = > `[${ this.nodeToString(x.to, true) }]`)
    //}`
    return NULL;
}