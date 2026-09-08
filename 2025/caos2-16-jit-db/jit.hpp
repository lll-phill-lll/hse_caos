#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include "expr.hpp"

using RowFn = uint64_t (*)(const uint64_t*);

class TJit {
public:
    TJit();
    ~TJit();

    void dumpObjectTo(std::string_view path);
    std::string buildIR(const TNode& root, int numCols);
    std::string optimize();
    RowFn compile();

private:
    struct TImpl;
    std::unique_ptr<TImpl> Impl;
};
