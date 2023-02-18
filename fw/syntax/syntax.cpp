#include "base/memory.h"
#include "syntax/syntax.h"

////////////////////////////////////////////////////////
//
//
//
//
//
//
//

using namespace NS_FW_SYNTAX;

Syntax::Syntax(void* grammar, bool debug) {
    //this.debug = debug;
    //this.grammar = grammar;

    //if (grammar) {
    //    // add the literal symbol for non-keyword terms
    //    this.literal = '_L';
    //    this.literalType = { 'symbol': 'L' }
    //    this.grammar.prototypes[this.literal] = this.literalType;
    //    // add the wildcard symbol
    //    this.wildcard = '_*';
    //    this.wildcardType = { 'symbol': '*' }
    //    this.grammar.prototypes[this.wildcard] = this.wildcardType;

    //    this.startTerm = null;

    //    // build list of symbols
    //    this.symbols = new Dictionary();

    //    for (var term in this.grammar.prototypes) {
    //        var type = this.grammar.prototypes[term]
    //            var symbol = type.symbol;
    //        if (!this.symbols.has(symbol)) {
    //            this.symbols.put(symbol, type);
    //        }
    //        if (type.start) this.startTerm = term;
    //    }
    //    this.literalCode = this.symbols.size - 2;
    //    this.wildcardCode = this.symbols.size - 1;
    //    // extract symbols from rules' outputs
    //    for (var ri = 0; ri < this.grammar.rules.length; ri++) {
    //        var rule = this.grammar.rules[ri];
    //        var symbols = rule.output != null ? rule.output.split(' ') : [''] ;
    //        if (symbols[0] != '') {
    //            for (var i = 0; i < symbols.length; i++) {
    //                if (!this.symbols.has(symbols[i])) {
    //                    this.symbols.put(symbols[i], { 'symbol':symbols[i] });
    //                }
    //            }
    //        }
    //        else {
    //            if (!this.symbols.has('')) {
    //                this.symbols.put('', { 'symbol':'', 'empty' : true });
    //            }
    //        }
    //    }
    //    // transform rules to use indices in the symbols array instead of symbols
    //    for (var rk = 0; rk < this.grammar.rules.length; rk++) {
    //        var rule = this.grammar.rules[rk];
    //        var symbols = rule.input.split(' ');
    //        rule.in = [];
    //        for (var i = 0; i < symbols.length; i++) {
    //            var ix = this.symbols.indexOf(symbols[i]);
    //            if (ix != -1) {
    //                rule.in.push(ix);
    //            }
    //            else {
    //                throw new Error(`Could not transform symbol '${symbols[i]}'!`);
    //            }
    //        }

    //        rule.out = null;
    //        if (rule.output != null) {
    //            symbols = rule.output.split(' ');
    //            if (symbols.length > 0) {
    //                rule.out = [];
    //                for (var i = 0; i < symbols.length; i++) {
    //                    var ix = this.symbols.indexOf(symbols[i]);
    //                    if (ix != -1) {
    //                        rule.out.push(ix);
    //                    }
    //                    else {
    //                        throw new Error(`Could not transform symbol '${symbols[i]}'!`);
    //                    }
    //                }
    //            }
    //        }
    //    }
    //    // sort rules by priority and input length
    //    this.ruleMap = this.grammar.rules.sort((a, b) = > 1000 * (b.priority - a.priority) + b.input.length - a.input.length);

    //    //console.log(this.ruleMap.map(r => rs(r)).join('\n'));
    //}
};

Expression* Syntax::createExpression() {
    return NEW_(Expression, this);
};
Expression* Syntax::parse(char* input, bool ignoreCase) {
    var expression = createExpression();
    //expression.expression = expr;
    //if (!expr || typeof expr != = 'string') {
    //    if (expr == '') {
    //        throw new Error('Empty expression!');
    //    }
    //    else {
    //        throw new Error(`Invalid expression!(${ expr })`);
    //    }
    //}

    //if (this.startTerm != null) {
    //    var type = this.grammar.prototypes[this.startTerm];
    //    expression.createNode(this.symbols.indexOf(type.symbol), this.startTerm, '<start>');
    //}
    //var start = 0, i = 0;
    //while (i < expr.length) {
    //    var hasMatch = false;
    //    var matched = expr.substring(i);
    //    if (ignoreCase) matched = matched.toLowerCase();
    //    for (var nk in this.grammar.prototypes) {
    //        if (matched.startsWith(nk)) {
    //            if (start < i) {
    //                var term = expr.substring(start, i);
    //                var code = this.symbols.indexOf(this.grammar.prototypes[this.literal].symbol);
    //                expression.createNode(code, this.literal, term);
    //            }
    //            if (this.grammar.prototypes[nk].symbol) {
    //                var code = this.symbols.indexOf(this.grammar.prototypes[nk].symbol);
    //                expression.createNode(code, nk, nk);
    //            }
    //            i += nk.length;
    //            start = i;
    //            hasMatch = true;
    //            break;
    //        }
    //    }
    //    if (!hasMatch) {
    //        i++;
    //    }
    //}
    //if (start < i) {
    //    var term = expr.substring(start, i);
    //    var code = this.symbols.indexOf(this.grammar.prototypes[this.literal].symbol);
    //    expression.createNode(code, this.literal, term);
    //    //nodes.push(this.createNode(code, this.literal, term));
    //}
    ////console.log('Nodes: ' + expression.tree.nodes.map(x => `${expression.nodeToString(x, true)}`).join('  '));
    return expression;
};