#include "syntax/expression.h"
#include "syntax/syntax.h"

using namespace NS_FW_SYNTAX;

Expression::Expression(Syntax* syntax) {
    syntax_ = syntax;
}

Expression* Expression::resolve(void* context) {
    return NULL;
}
void* Expression::evaluate(void* context) {
    return NULL;
}
Map* Expression::createInOutMap(PArray* ruleIn, PArray* ruleOut, Array* missing) {
    return NULL;
}
PArray* Expression::mergeNodes(PArray* nodes, Map* inOutMap) {
    return NULL;
}
PArray* Expression::shuffleNodes(PArray* nodes, Map* inOutMap) {
    return NULL;
}
void Expression::applyRule(Rule* rule, PArray* nodes, int pos, void* context) {
}
bool Expression::matchRule(Rule* rule, PArray* nodes, void* context) {
    return true;
}
Node* Expression::createNode(int code, void* type, char* term) {
    return NULL;
}

//Expression.prototype.createInOutMap = function createInOutMap(ruleIn, ruleOut, missing) {
//    var inOutMap = new Dictionary();
//    var arr = Array.from(ruleOut);
//    for (var i = 0; i < ruleIn.length; i++) {
//        var found = false;
//        for (var j = 0; j < arr.length; j++) {
//            if (ruleIn[i] == arr[j] && arr[j] != this.syntax.literalCode) {
//                inOutMap.put(i, j);
//                arr[j] = null;
//                found = true;
//                break;
//            }
//        }
//        if (!found) {
//            missing.push(i);
//        }
//    }
//    return inOutMap;
//};
//Expression.prototype.mergeNodes = function mergeNodes(nodes, inOutMap) {
//    if (this.syntax.debug > 2) console.log('merge in: ' + nodes.map(n = > `{${ this.nodeToString(n) }
//}`).join('  '));
//    var parents = nodes.map(() = > 0);
//    for (var i = 0; i < nodes.length; i++) {
//        var node = nodes[i];
//        // candidate for parent:
//        // - appears on both sides of the rule, strongest rule
//        if (inOutMap.has(i)) parents[i] += 10;
//        // - has an edge
//        if (node.edges.length > 0) parents[i]++;
//        // - has an action
//        if (node.data.type.action != null) parents[i]++;
//        // - non-literal node
//        if (node.data.code == this.syntax.literalCode) parents[i] -= 8;
//        // - is a start node
//        if (node.data.type.start) parents[i]++;
//        // - not ignored
//        if (node.data.type.ignore) parents[i] -= 10;
//    }
//    // - leftmost node wins
//    var pi = 0;
//    for (var i = 1; i < parents.length; i++) {
//        if (parents[i] > parents[pi]) pi = i;
//    }
//    var parent = nodes[pi];
//    var remains = [];
//    var ci = nodes.length - inOutMap.size;
//    for (var i = 0; i < nodes.length; i++) {
//        if (i == pi || inOutMap.has(i)) {
//            remains.push(nodes[i]);
//            continue;
//        }
//        else if (!nodes[i].data.type.ignore) {
//            this.tree.addEdge(parent, nodes[i]);
//            nodes[i] = null;
//        }
//        if (ci-- == 0) break;
//    }
//    if (this.syntax.debug > 2) console.log('merge out: ' + remains.map(n = > `{${ this.nodeToString(n) }
//}`).join('  '));
//    return remains;
//    };
//
//    Expression.prototype.shuffleNodes = function shuffleNodes(nodes, inOutMap) {
//        var arr = new Array(nodes.length);
//        for (var j = 0; j < nodes.length; j++) {
//            var ix = inOutMap.has(j) ? inOutMap.get(j) : j;
//            arr[ix] = nodes[j];
//        }
//        return arr;
//    };
//    Expression.prototype.applyRule = function applyRule(rule, nodes, n, context) {
//        // match found, replace input by output
//        var i = rule.in.length;
//        if (this.syntax.debug > 1) console.log(`match: ${ rs(rule) }`);
//        // extract rule's input
//        var inNodes = nodes.splice(n, i);
//        // get output
//        var ruleOut = rule.out;
//        if (typeof rule.action == = 'function') {
//            var outNodes = rule.action.apply(context, inNodes);
//            if (outNodes != null) {
//                if (!Array.isArray(outNodes)) outNodes = [outNodes];
//                ruleOut = outNodes.map(x = > x.data.code);
//            }
//        }
//        if (rule.in.length < ruleOut.length) {
//            throw new Error('Invalid rule: output cannot be longer than input!');
//        }
//        var outNodes = [];
//        if (!this.syntax.symbols.getAt(ruleOut[0]).empty) {
//            var missing = [];
//            outNodes = inNodes;
//            // get mapping between in-out symbols and missing symbols
//            var inOutMap = this.createInOutMap(rule.in, ruleOut, missing);
//            if (rule.in.length > ruleOut.length) {
//                // merge nodes
//                outNodes = this.mergeNodes(inNodes, inOutMap);
//            }
//
//            // shuffle nodes
//            if (inOutMap.size > 0) {
//                outNodes = this.shuffleNodes(outNodes, inOutMap);
//            }
//
//            // relabel nodes
//            if (outNodes.length > 0) {
//                for (var i = 0; i < ruleOut.length; i++) {
//                    var code = ruleOut[i];
//                    outNodes[i].data.code = code;
//                    //outNodes[i].data.type = this.syntax.symbols.getAt(code);
//                }
//            }
//        }
//        // re-insert the processed nodes
//        nodes.splice(n, 0, ...outNodes);
//        if (nodes[0]) {
//            this.lastNode = nodes[0];
//        }
//
//        //if (this.syntax.debug > 1) console.log(`result: ${nodes.map(x => `${this.syntax.symbols.getAt(x.data.code).symbol}['${x.data.term.replace(/[\n\r]/g, '\\n')}']`)}`);
//        if (this.syntax.debug > 0) console.log('result: ' + nodes.map(n = > `{${ this.nodeToString(n) }
//    }`).join('  '));
//    };
//    Expression.prototype.matchRule = function matchRule(rule, nodes, context) {
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
//    };
//    Expression.prototype.resolve = function(context) {
//        this.lastNode = null;
//        var nodes = Array.from(this.tree.vertices);
//        if (this.syntax.debug > 0) console.log(`input: ${ nodes.map(x = > `${x.data.type.symbol } ['${x.data.term.replace(/[\n\r]/g, '\\n')}'] `)
//    }`);
//    // try to apply rules as long as there are nodes
//    while (nodes.length > 0) {
//        var r = 0;
//        while (r < this.syntax.ruleMap.length) {
//            var rule = this.syntax.ruleMap[r];
//            if (this.matchRule(rule, nodes, context)) {
//                // has a match, start over
//                // TODO: sort rules by prio and input length
//                r = 0;
//            }
//            else {
//                // has no match, try the next rule
//                r++;
//            }
//        }
//        break;
//    }
//    return this;
//    };
//    Expression.prototype.evaluate = function(context) {
//        this.tree.DFS(this.lastNode, null, n = > {
//            // node handler calls action with the input nodes as arguments
//            if (typeof n.data.type.action == = 'function') {
//                var args = n.edges.map(x = > x.to);
//                // action returns a (calculated) value of the node
//                args.unshift(n);
//                n.data.value = n.data.type.action.apply(context, args);
//            }
//        });
//        return this.lastNode.data;  //.value;
//    };
//    Expression.prototype.nodeToString = function(node, simple) {
//        var value = typeof node.data.value != = 'object' ? node.data.value.toString() : node.data.term;
//        value = value.replace(/ [\n\r] / g, '\\n');
//        var text = `#${ node.id }:'${value}'(${ this.syntax.symbols.getAt(node.data.code).symbol }:${ node.data.type.symbol })`;
//            if (!simple) {
//                text += `${node.edges.map(x = > `[${ this.nodeToString(x.to, true) }]`)
//            }`;
//    }
//    return text;
//    };
//    Expression.prototype.createNode = function(code, type, term) {
//        var node = new Node(code, this.syntax.grammar.prototypes[type], term);
//        return node.vertex = this.tree.createVertex(node);
//    };