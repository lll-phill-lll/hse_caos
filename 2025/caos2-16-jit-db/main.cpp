#include <chrono>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "expr.hpp"
#include "interp.hpp"
#include "jit.hpp"
#include "cppjit.hpp"

using Clock = std::chrono::steady_clock;
using Data = std::vector<uint64_t>;

double seconds(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration<double>(b - a).count();
}

template <class Eval>
double runEngine(Eval&& eval, const Data& data, size_t rows, int numCols, uint64_t& sumOut) {
    std::span<const uint64_t> all(data);
    size_t cols = (size_t)numCols;
    auto t0 = Clock::now();
    uint64_t acc = 0;
    for (size_t r = 0; r < rows; ++r)
        acc += eval(all.subspan(r * cols, cols));
    sumOut = acc;
    return seconds(t0, Clock::now());
}

void writeFile(std::string_view path, std::string_view s) {
    std::ofstream(std::string(path)) << s;
}

struct InterpResult {
    double time = 0;
    uint64_t sum = 0;
};

struct JitResult {
    double time = 0;
    uint64_t sum = 0;
    double compile = 0, build = 0, opt = 0, backend = 0;
};

struct CppResult {
    double time = 0;
    uint64_t sum = 0;
    double compile = 0;
    bool ok = false;
};

InterpResult runInterpreter(const TNode& ast, const Data& data, size_t rows, int numCols) {
    OpPtr opTree = buildOperator(ast);
    InterpResult r;
    r.time = runEngine([&](Row row) { return opTree->eval(row); }, data, rows, numCols, r.sum);
    return r;
}

JitResult runJit(const TNode& ast, const Data& data, size_t rows, int numCols) {
    TJit jit;
    jit.dumpObjectTo("jit_query.o");

    auto c0 = Clock::now();
    std::string irUnopt = jit.buildIR(ast, numCols);
    auto c1 = Clock::now();
    std::string irOpt = jit.optimize();
    auto c2 = Clock::now();
    RowFn fn = jit.compile();
    auto c3 = Clock::now();

    writeFile("query.ll", irUnopt);
    writeFile("query.opt.ll", irOpt);

    JitResult r;
    r.build = seconds(c0, c1);
    r.opt = seconds(c1, c2);
    r.backend = seconds(c2, c3);
    r.compile = seconds(c0, c3);
    r.time = runEngine([&](Row row) { return fn(row.data()); }, data, rows, numCols, r.sum);
    return r;
}

CppResult runJitCpp(const TNode& ast, const Data& data, size_t rows, int numCols) {
    TCppJit cpp;
    std::string src = cpp.genSource(ast, numCols);
    cpp.writeSource(src);

    auto d0 = Clock::now();
    bool compiled = cpp.compileLib();
    auto d1 = Clock::now();

    CppResult r;
    r.compile = seconds(d0, d1);
    RowFn fn = compiled ? cpp.load() : nullptr;
    r.ok = fn != nullptr;
    if (fn)
        r.time = runEngine([&](Row row) { return fn(row.data()); }, data, rows, numCols, r.sum);
    return r;
}

int main(int argc, char** argv) {
    size_t rows = 30'000'000;
    uint64_t seed = 42;
    std::string_view exprStr;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "--rows" && i + 1 < argc)      rows = std::stoull(argv[++i]);
        else if (arg == "--seed" && i + 1 < argc) seed = std::stoull(argv[++i]);
        else                                      exprStr = argv[i];
    }
    if (exprStr.empty()) {
        std::cerr << std::format("usage: {} [--rows N] [--seed S] \"<expression>\"\n", argv[0]);
        return 2;
    }

    int numCols = 0;
    NodePtr ast = parseExpr(exprStr, numCols);
    if (numCols <= 0) numCols = 1;

    std::cout << std::format("expression : {}\n", exprStr);
    std::cout << std::format("rows       : {}\n", rows);
    std::cout << std::format("cols       : {}\n", numCols);
    std::cout << std::format("data       : {:.2f} GB\n\n",
                             (double)rows * numCols * sizeof(uint64_t) / 1e9);

    std::string norm, tree;
    astToString(*ast, norm);
    dumpAstTree(*ast, tree);
    std::cout << std::format("parsed     : {}\nAST:\n{}\n", norm, tree);

    Data data((size_t)rows * numCols);
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<uint64_t> dist(0, 100);
    for (auto& x : data) x = dist(rng);

    InterpResult interp = runInterpreter(*ast, data, rows, numCols);
    CppResult cpp = runJitCpp(*ast, data, rows, numCols);
    JitResult jit = runJit(*ast, data, rows, numCols);

    auto mrows = [&](double t) { return t > 0 ? (double)rows / t / 1e6 : 0.0; };

    std::cout << std::format("{:<22}{:>10}{:>11}   {}\n", "engine", "time, s", "Mrows/s", "SUM");
    std::cout << std::format("{:<22}{:>10.3f}{:>11.1f}   {}\n", "interpreter", interp.time, mrows(interp.time), interp.sum);
    std::cout << std::format("{:<22}{:>10.3f}{:>11.1f}   {}\n", "LLVM JIT", jit.time, mrows(jit.time), jit.sum);
    if (cpp.ok)
        std::cout << std::format("{:<22}{:>10.3f}{:>11.1f}   {}\n", "C++ -> .so", cpp.time, mrows(cpp.time), cpp.sum);
    else
        std::cout << "C++ -> .so (compile failed)\n";

    std::cout << std::format("\nLLVM JIT {:.3f} ms (build {:.3f} + opt {:.3f} + backend {:.3f})\n",
                             jit.compile * 1e3, jit.build * 1e3, jit.opt * 1e3, jit.backend * 1e3);
    if (cpp.ok)
        std::cout << std::format("-> .so {:.1f} ms\n",
                                 cpp.compile * 1e3);

    bool ok = (interp.sum == jit.sum) && (!cpp.ok || interp.sum == cpp.sum);
    std::cout << std::format("match: {}\n", ok ? "OK" : "MISMATCH");
    return 0;
}
