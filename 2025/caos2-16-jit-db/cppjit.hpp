#pragma once
#include <string>
#include <string_view>
#include "expr.hpp"
#include "jit.hpp"

class TCppJit {
public:
    TCppJit() = default;
    ~TCppJit();

    std::string genSource(const TNode& root, int numCols);
    void writeSource(std::string_view src);
    bool compileLib();
    RowFn load();

    const std::string& srcPath() const { return SrcPath; }
    const std::string& soPath()  const { return SoPath; }
    const std::string& lastCmd() const { return LastCmd; }

private:
    std::string SrcPath = "query_gen.cpp";
    std::string SoPath  = "./query_gen.so";
    std::string LastCmd;
    void* Handle = nullptr;
};
