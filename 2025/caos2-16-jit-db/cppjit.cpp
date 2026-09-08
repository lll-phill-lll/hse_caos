#include "cppjit.hpp"

#include <cstdlib>
#include <dlfcn.h>
#include <format>
#include <fstream>
#include <iostream>
#include <string_view>

void genCpp(const TNode& n, std::string& out) {
    auto cmp = [&](std::string_view op) {
        out += "((";
        genCpp(*n.kids[0], out);
        out += ") "; out += op; out += " (";
        genCpp(*n.kids[1], out);
        out += ") ? 1 : 0)";
    };
    auto bin = [&](std::string_view op) {
        out += "(";
        genCpp(*n.kids[0], out);
        out += " "; out += op; out += " ";
        genCpp(*n.kids[1], out);
        out += ")";
    };
    auto fn2 = [&](std::string_view name) {
        out += name; out += "(";
        genCpp(*n.kids[0], out); out += ", ";
        genCpp(*n.kids[1], out); out += ")";
    };
    switch (n.op) {
        case Op::Num: out += std::format("{}ULL", n.num); break;
        case Op::Col: out += std::format("row[{}]", n.col); break;
        case Op::Neg: out += "(-("; genCpp(*n.kids[0], out); out += "))"; break;
        case Op::Add: bin("+"); break;
        case Op::Sub: bin("-"); break;
        case Op::Mul: bin("*"); break;
        case Op::Lt:  cmp("<");  break;
        case Op::Gt:  cmp(">");  break;
        case Op::Le:  cmp("<="); break;
        case Op::Ge:  cmp(">="); break;
        case Op::Eq:  cmp("=="); break;
        case Op::Ne:  cmp("!="); break;
        case Op::Min: fn2("std::min<uint64_t>"); break;
        case Op::Max: fn2("std::max<uint64_t>"); break;
        case Op::Select:
            out += "(((";
            genCpp(*n.kids[0], out);
            out += ") != 0) ? (";
            genCpp(*n.kids[1], out);
            out += ") : (";
            genCpp(*n.kids[2], out);
            out += "))";
            break;
    }
}

std::string TCppJit::genSource(const TNode& root, int numCols) {
    std::string expr;
    genCpp(root, expr);
    return std::format(
        "// generated from query (numCols={})\n"
        "#include <algorithm>\n"
        "#include <cstdint>\n\n"
        "extern \"C\" uint64_t query(const uint64_t* row) {{\n"
        "    return {};\n"
        "}}\n",
        numCols, expr);
}

void TCppJit::writeSource(std::string_view src) {
    std::ofstream f(SrcPath);
    f << src;
}

bool TCppJit::compileLib() {
    LastCmd = std::format("clang++ -O2 -march=native -shared -fPIC -o {} {}",
                          SoPath, SrcPath);
    return std::system(LastCmd.c_str()) == 0;
}

RowFn TCppJit::load() {
    Handle = dlopen(SoPath.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!Handle) {
        std::cerr << std::format("dlopen failed: {}\n", dlerror());
        return nullptr;
    }
    dlerror();
    void* sym = dlsym(Handle, "query");
    if (const char* err = dlerror()) {
        std::cerr << std::format("dlsym failed: {}\n", err);
        return nullptr;
    }
    return reinterpret_cast<RowFn>(sym);
}

TCppJit::~TCppJit() {
    if (Handle) dlclose(Handle);
}
