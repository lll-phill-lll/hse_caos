#pragma once
#include <cstdint>
#include <memory>
#include <span>
#include "expr.hpp"

using Row = std::span<const uint64_t>;

struct TOperator {
    virtual ~TOperator() = default;
    virtual uint64_t eval(Row row) const = 0;
};

using OpPtr = std::unique_ptr<TOperator>;

struct TConstOp : TOperator {
    uint64_t v;
    explicit TConstOp(uint64_t v) : v(v) {}
    uint64_t eval(Row) const override { return v; }
};

struct TColOp : TOperator {
    int idx;
    explicit TColOp(int i) : idx(i) {}
    uint64_t eval(Row row) const override { return row[idx]; }
};

struct TNegOp : TOperator {
    OpPtr a;
    explicit TNegOp(OpPtr a) : a(std::move(a)) {}
    uint64_t eval(Row row) const override { return -a->eval(row); }
};

template <class F>
struct TBinOp : TOperator {
    OpPtr a, b;
    TBinOp(OpPtr a, OpPtr b) : a(std::move(a)), b(std::move(b)) {}
    uint64_t eval(Row row) const override { return F{}(a->eval(row), b->eval(row)); }
};

struct TSelectOp : TOperator {
    OpPtr c, t, e;
    TSelectOp(OpPtr c, OpPtr t, OpPtr e) : c(std::move(c)), t(std::move(t)), e(std::move(e)) {}
    uint64_t eval(Row row) const override {
        return c->eval(row) != 0 ? t->eval(row) : e->eval(row);
    }
};

struct TFAdd { uint64_t operator()(uint64_t a, uint64_t b) const { return a + b; } };
struct TFSub { uint64_t operator()(uint64_t a, uint64_t b) const { return a - b; } };
struct TFMul { uint64_t operator()(uint64_t a, uint64_t b) const { return a * b; } };
struct TFLt  { uint64_t operator()(uint64_t a, uint64_t b) const { return a <  b ? 1 : 0; } };
struct TFGt  { uint64_t operator()(uint64_t a, uint64_t b) const { return a >  b ? 1 : 0; } };
struct TFLe  { uint64_t operator()(uint64_t a, uint64_t b) const { return a <= b ? 1 : 0; } };
struct TFGe  { uint64_t operator()(uint64_t a, uint64_t b) const { return a >= b ? 1 : 0; } };
struct TFEq  { uint64_t operator()(uint64_t a, uint64_t b) const { return a == b ? 1 : 0; } };
struct TFNe  { uint64_t operator()(uint64_t a, uint64_t b) const { return a != b ? 1 : 0; } };
struct TFMin { uint64_t operator()(uint64_t a, uint64_t b) const { return a < b ? a : b; } };
struct TFMax { uint64_t operator()(uint64_t a, uint64_t b) const { return a > b ? a : b; } };

inline OpPtr buildOperator(const TNode& n) {
    auto k = [&](int i) { return buildOperator(*n.kids[i]); };
    switch (n.op) {
        case Op::Num: return std::make_unique<TConstOp>(n.num);
        case Op::Col: return std::make_unique<TColOp>(n.col);
        case Op::Neg: return std::make_unique<TNegOp>(k(0));
        case Op::Add: return std::make_unique<TBinOp<TFAdd>>(k(0), k(1));
        case Op::Sub: return std::make_unique<TBinOp<TFSub>>(k(0), k(1));
        case Op::Mul: return std::make_unique<TBinOp<TFMul>>(k(0), k(1));
        case Op::Lt:  return std::make_unique<TBinOp<TFLt>>(k(0), k(1));
        case Op::Gt:  return std::make_unique<TBinOp<TFGt>>(k(0), k(1));
        case Op::Le:  return std::make_unique<TBinOp<TFLe>>(k(0), k(1));
        case Op::Ge:  return std::make_unique<TBinOp<TFGe>>(k(0), k(1));
        case Op::Eq:  return std::make_unique<TBinOp<TFEq>>(k(0), k(1));
        case Op::Ne:  return std::make_unique<TBinOp<TFNe>>(k(0), k(1));
        case Op::Min: return std::make_unique<TBinOp<TFMin>>(k(0), k(1));
        case Op::Max: return std::make_unique<TBinOp<TFMax>>(k(0), k(1));
        case Op::Select: return std::make_unique<TSelectOp>(k(0), k(1), k(2));
    }
    return std::make_unique<TConstOp>(0);
}
