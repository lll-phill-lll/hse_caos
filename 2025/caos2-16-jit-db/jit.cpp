#include "jit.hpp"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Error.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/ExecutionEngine/Orc/ObjectTransformLayer.h"
#include "llvm/Support/MemoryBuffer.h"
using namespace llvm;

struct TJit::TImpl {
    std::unique_ptr<orc::LLJIT> jit;
    std::unique_ptr<LLVMContext> ctx;
    std::unique_ptr<Module> mod;
    IRBuilder<>* builder = nullptr;
    Function* fn = nullptr;
    Value* rowArg = nullptr;
    int numCols = 0;

    Value* gen(const TNode& n) {
        IRBuilder<>& B = *builder;
        Type* i64 = B.getInt64Ty();

        switch (n.op) {
            case Op::Num:
                return B.getInt64(n.num);

            case Op::Col: {
                Value* idx = B.getInt64(n.col);
                Value* ptr = B.CreateGEP(i64, rowArg, idx, "colptr");
                return B.CreateLoad(i64, ptr, "col");
            }

            case Op::Neg: return B.CreateNeg(gen(*n.kids[0]), "neg");

            case Op::Add: return B.CreateAdd(gen(*n.kids[0]), gen(*n.kids[1]), "add");
            case Op::Sub: return B.CreateSub(gen(*n.kids[0]), gen(*n.kids[1]), "sub");
            case Op::Mul: return B.CreateMul(gen(*n.kids[0]), gen(*n.kids[1]), "mul");

            case Op::Lt: return cmp(CmpInst::ICMP_ULT, n);
            case Op::Gt: return cmp(CmpInst::ICMP_UGT, n);
            case Op::Le: return cmp(CmpInst::ICMP_ULE, n);
            case Op::Ge: return cmp(CmpInst::ICMP_UGE, n);
            case Op::Eq: return cmp(CmpInst::ICMP_EQ, n);
            case Op::Ne: return cmp(CmpInst::ICMP_NE, n);

            case Op::Min:
                return B.CreateBinaryIntrinsic(Intrinsic::umin, gen(*n.kids[0]), gen(*n.kids[1]));
            case Op::Max:
                return B.CreateBinaryIntrinsic(Intrinsic::umax, gen(*n.kids[0]), gen(*n.kids[1]));

            case Op::Select: {
                Value* cond = B.CreateICmpNE(gen(*n.kids[0]), B.getInt64(0), "cond");
                Value* t = gen(*n.kids[1]);
                Value* e = gen(*n.kids[2]);
                return B.CreateSelect(cond, t, e, "sel");
            }
        }
        return B.getInt64(0);
    }

    Value* cmp(CmpInst::Predicate pred, const TNode& n) {
        IRBuilder<>& B = *builder;
        Value* a = gen(*n.kids[0]);
        Value* b = gen(*n.kids[1]);
        Value* i1 = B.CreateICmp(pred, a, b, "cmp");
        return B.CreateZExt(i1, B.getInt64Ty(), "toi64");
    }
};

TJit::TJit() : Impl(std::make_unique<TImpl>()) {
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    Impl->jit = cantFail(orc::LLJITBuilder().create());
    Impl->ctx = std::make_unique<LLVMContext>();
}

TJit::~TJit() = default;

void TJit::dumpObjectTo(std::string_view path) {
    Impl->jit->getObjTransformLayer().setTransform(
        [path = std::string(path)](std::unique_ptr<MemoryBuffer> obj)
            -> Expected<std::unique_ptr<MemoryBuffer>> {
            std::error_code ec;
            raw_fd_ostream os(path, ec);
            if (!ec) os << obj->getBuffer();
            return obj;
        });
}

std::string TJit::buildIR(const TNode& root, int numCols) {
    TImpl& s = *Impl;
    s.numCols = numCols;
    s.mod = std::make_unique<Module>("query_module", *s.ctx);
    s.mod->setDataLayout(s.jit->getDataLayout());
    s.mod->setTargetTriple(s.jit->getTargetTriple());

    IRBuilder<> builder(*s.ctx);
    s.builder = &builder;

    Type* i64 = builder.getInt64Ty();
    Type* ptr = PointerType::getUnqual(*s.ctx);
    FunctionType* fty = FunctionType::get(i64, {ptr}, false);
    s.fn = Function::Create(fty, Function::ExternalLinkage, "query", s.mod.get());
    s.rowArg = s.fn->getArg(0);
    s.rowArg->setName("row");

    BasicBlock* bb = BasicBlock::Create(*s.ctx, "entry", s.fn);
    builder.SetInsertPoint(bb);

    builder.CreateRet(s.gen(root));

    if (verifyFunction(*s.fn, &errs()))
        errs() << "internal error: invalid IR generated\n";

    std::string out;
    raw_string_ostream os(out);
    s.mod->print(os, nullptr);
    return out;
}

std::string TJit::optimize() {
    TImpl& s = *Impl;
    PassBuilder PB;
    LoopAnalysisManager LAM;
    FunctionAnalysisManager FAM;
    CGSCCAnalysisManager CGAM;
    ModuleAnalysisManager MAM;
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(OptimizationLevel::O2);
    MPM.run(*s.mod, MAM);

    std::string out;
    raw_string_ostream os(out);
    s.mod->print(os, nullptr);
    return out;
}

RowFn TJit::compile() {
    TImpl& s = *Impl;
    orc::ThreadSafeModule tsm(std::move(s.mod), std::move(s.ctx));
    cantFail(s.jit->addIRModule(std::move(tsm)));
    auto sym = cantFail(s.jit->lookup("query"));
    return sym.toPtr<RowFn>();
}
