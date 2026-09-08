#pragma once
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/*
 * ebnf
grammar Expr;
prog    : ternary EOF ;
ternary : compare ('?' ternary ':' ternary)? ;
compare : add (relop add)? ;                 
add     : mul (('+' | '-') mul)* ;          
mul     : unary ('*' unary)* ;             
unary   : ('-' | '+') unary | primary ;   
primary : NUM
        | COL
        | func '(' ternary ',' ternary ')'
        | '(' ternary ')'
        ;
relop   : '<' | '<=' | '>' | '>=' | '==' | '!=' ;
func    : 'min' | 'max' ;

NUM : [0-9]+ ;
COL : 'c' [0-9]+ ;
WS  : [ \t\r\n]+ -> skip ;
*/

enum class Op {
    Num, Col, Neg,
    Add, Sub, Mul,
    Lt, Gt, Le, Ge, Eq, Ne,
    Min, Max,
    Select,
};

struct TNode {
    Op op;
    uint64_t num = 0;
    int col = -1;
    std::vector<std::unique_ptr<TNode>> kids;
};

using NodePtr = std::unique_ptr<TNode>;

enum class Tok { End, Num, Col, Func, LParen, RParen, Quest, Colon, Comma,
                 Plus, Minus, Star, Lt, Gt, Le, Ge, Eq, Ne };

struct TToken {
    Tok kind;
    uint64_t num = 0;
    int col = -1;
    Op func = Op::Min;
};

[[noreturn]] inline void parseError(std::string_view src, std::string_view msg) {
    std::cerr << std::format("parse error: {}\n  in expression: {}\n", msg, src);
    std::exit(1);
}

inline std::vector<TToken> lex(std::string_view src) {
    std::vector<TToken> out;
    auto push = [&](Tok k) { out.push_back(TToken{k}); };
    auto digit = [](char c) { return std::isdigit((unsigned char)c) != 0; };
    auto alpha = [](char c) { return std::isalpha((unsigned char)c) != 0; };

    size_t i = 0, n = src.size();
    while (i < n) {
        char c = src[i];
        if (std::isspace((unsigned char)c)) { ++i; continue; }

        if (digit(c)) {
            uint64_t v = 0;
            while (i < n && digit(src[i])) v = v * 10 + (uint64_t)(src[i++] - '0');
            TToken t{Tok::Num}; t.num = v; out.push_back(t); continue;
        }
        if (c == 'c' && i + 1 < n && digit(src[i + 1])) {
            ++i; int idx = 0;
            while (i < n && digit(src[i])) idx = idx * 10 + (src[i++] - '0');
            TToken t{Tok::Col}; t.col = idx; out.push_back(t); continue;
        }
        if (alpha(c)) {
            size_t s = i;
            while (i < n && alpha(src[i])) ++i;
            std::string_view id = src.substr(s, i - s);
            TToken t{Tok::Func};
            if (id == "min") t.func = Op::Min;
            else if (id == "max") t.func = Op::Max;
            else parseError(src, std::format("unknown name '{}'", id));
            out.push_back(t); continue;
        }

        char nx = (i + 1 < n) ? src[i + 1] : '\0';
        switch (c) {
            case '(': push(Tok::LParen); ++i; break;
            case ')': push(Tok::RParen); ++i; break;
            case '?': push(Tok::Quest);  ++i; break;
            case ':': push(Tok::Colon);  ++i; break;
            case ',': push(Tok::Comma);  ++i; break;
            case '+': push(Tok::Plus);   ++i; break;
            case '-': push(Tok::Minus);  ++i; break;
            case '*': push(Tok::Star);   ++i; break;
            case '<': if (nx == '=') { push(Tok::Le); i += 2; } else { push(Tok::Lt); ++i; } break;
            case '>': if (nx == '=') { push(Tok::Ge); i += 2; } else { push(Tok::Gt); ++i; } break;
            case '=': if (nx == '=') { push(Tok::Eq); i += 2; } else parseError(src, "expected '=='"); break;
            case '!': if (nx == '=') { push(Tok::Ne); i += 2; } else parseError(src, "expected '!='"); break;
            default:  parseError(src, "unexpected character");
        }
    }
    push(Tok::End);
    return out;
}

class TParser {
public:
    explicit TParser(std::string_view src) : Src(src), Toks(lex(src)) {}

    NodePtr parse(int& numCols) {
        NodePtr n = ternary();
        if (cur().kind != Tok::End) parseError(Src, "trailing characters");
        numCols = MaxCol + 1;
        return n;
    }

private:
    const TToken& cur() const { return Toks[Pos]; }
    bool accept(Tok k) { if (cur().kind == k) { ++Pos; return true; } return false; }

    static NodePtr make(Op op) { auto n = std::make_unique<TNode>(); n->op = op; return n; }
    static NodePtr bin(Op op, NodePtr a, NodePtr b) {
        auto n = make(op); n->kids.push_back(std::move(a)); n->kids.push_back(std::move(b)); return n;
    }

    NodePtr ternary() {
        NodePtr cond = compare();
        if (accept(Tok::Quest)) {
            NodePtr a = ternary();
            if (!accept(Tok::Colon)) parseError(Src, "expected ':' in ternary");
            NodePtr b = ternary();
            auto n = make(Op::Select);
            n->kids.push_back(std::move(cond));
            n->kids.push_back(std::move(a));
            n->kids.push_back(std::move(b));
            return n;
        }
        return cond;
    }

    NodePtr compare() {
        NodePtr a = add();
        Op op;
        switch (cur().kind) {
            case Tok::Lt: op = Op::Lt; break;
            case Tok::Gt: op = Op::Gt; break;
            case Tok::Le: op = Op::Le; break;
            case Tok::Ge: op = Op::Ge; break;
            case Tok::Eq: op = Op::Eq; break;
            case Tok::Ne: op = Op::Ne; break;
            default: return a;
        }
        ++Pos;
        return bin(op, std::move(a), add());
    }

    NodePtr add() {
        NodePtr a = mul();
        for (;;) {
            if (accept(Tok::Plus))       a = bin(Op::Add, std::move(a), mul());
            else if (accept(Tok::Minus)) a = bin(Op::Sub, std::move(a), mul());
            else return a;
        }
    }

    NodePtr mul() {
        NodePtr a = unary();
        while (accept(Tok::Star)) a = bin(Op::Mul, std::move(a), unary());
        return a;
    }

    NodePtr unary() {
        if (accept(Tok::Minus)) { auto n = make(Op::Neg); n->kids.push_back(unary()); return n; }
        if (accept(Tok::Plus))  return unary();
        return primary();
    }

    NodePtr primary() {
        const TToken& t = cur();
        if (t.kind == Tok::Num) { ++Pos; auto n = make(Op::Num); n->num = t.num; return n; }
        if (t.kind == Tok::Col) {
            ++Pos; auto n = make(Op::Col); n->col = t.col;
            if (t.col > MaxCol) MaxCol = t.col;
            return n;
        }
        if (t.kind == Tok::Func) {
            Op fop = t.func; ++Pos;
            if (!accept(Tok::LParen)) parseError(Src, "expected '(' after function");
            NodePtr a = ternary();
            if (!accept(Tok::Comma)) parseError(Src, "expected ',' between function arguments");
            NodePtr b = ternary();
            if (!accept(Tok::RParen)) parseError(Src, "expected ')'");
            return bin(fop, std::move(a), std::move(b));
        }
        if (accept(Tok::LParen)) {
            NodePtr n = ternary();
            if (!accept(Tok::RParen)) parseError(Src, "expected ')'");
            return n;
        }
        parseError(Src, "expected number, column cN or '('");
    }

    std::string_view Src;
    std::vector<TToken> Toks;
    size_t Pos = 0;
    int MaxCol = -1;
};

inline NodePtr parseExpr(std::string_view src, int& numCols) {
    TParser p(src);
    return p.parse(numCols);
}

inline std::string_view opName(Op op) {
    switch (op) {
        case Op::Num: return "Num";   case Op::Col: return "Col";
        case Op::Neg: return "Neg";   case Op::Add: return "Add";
        case Op::Sub: return "Sub";   case Op::Mul: return "Mul";
        case Op::Lt:  return "Lt";    case Op::Gt:  return "Gt";
        case Op::Le:  return "Le";    case Op::Ge:  return "Ge";
        case Op::Eq:  return "Eq";    case Op::Ne:  return "Ne";
        case Op::Min: return "Min";   case Op::Max: return "Max";
        case Op::Select: return "Select";
    }
    return "?";
}

inline std::string_view opSymbol(Op op) {
    switch (op) {
        case Op::Add: return "+"; case Op::Sub: return "-";
        case Op::Mul: return "*";
        case Op::Lt:  return "<"; case Op::Gt:  return ">";
        case Op::Le:  return "<="; case Op::Ge: return ">=";
        case Op::Eq:  return "=="; case Op::Ne: return "!=";
        default: return "?";
    }
}

inline void dumpAstNode(const TNode& n, std::string& out, std::string_view prefix, bool last, bool root) {
    out += prefix;
    if (!root) out += (last ? "\u2514\u2500 " : "\u251c\u2500 ");
    switch (n.op) {
        case Op::Num: out += std::format("Num {}", n.num); break;
        case Op::Col: out += std::format("Col c{}", n.col); break;
        default:      out += opName(n.op); break;
    }
    out += "\n";
    std::string childPrefix = root ? std::string{} : std::string(prefix) + (last ? "   " : "\u2502  ");
    for (size_t i = 0; i < n.kids.size(); ++i)
        dumpAstNode(*n.kids[i], out, childPrefix, i + 1 == n.kids.size(), false);
}

inline void dumpAstTree(const TNode& n, std::string& out) {
    dumpAstNode(n, out, "", true, true);
}

inline void astToString(const TNode& n, std::string& out) {
    switch (n.op) {
        case Op::Num: out += std::format("{}", n.num); break;
        case Op::Col: out += std::format("c{}", n.col); break;
        case Op::Neg: out += "(-"; astToString(*n.kids[0], out); out += ")"; break;
        case Op::Min:
            out += "min("; astToString(*n.kids[0], out); out += ", ";
            astToString(*n.kids[1], out); out += ")"; break;
        case Op::Max:
            out += "max("; astToString(*n.kids[0], out); out += ", ";
            astToString(*n.kids[1], out); out += ")"; break;
        case Op::Select:
            out += "(";
            astToString(*n.kids[0], out); out += " ? ";
            astToString(*n.kids[1], out); out += " : ";
            astToString(*n.kids[2], out); out += ")";
            break;
        default:
            out += "(";
            astToString(*n.kids[0], out);
            out += " "; out += opSymbol(n.op); out += " ";
            astToString(*n.kids[1], out);
            out += ")";
            break;
    }
}
