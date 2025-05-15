#pragma warning(disable:4244 4722 4267 4146 4624)

#include <iostream>
#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/LLVMContext.h>

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>

// #include <llvm/AsmParser/Parser.h>
#include <iostream>

using namespace llvm;
using namespace llvm::orc;
using namespace std;

int Execute(LLJIT& jit, unique_ptr<Module> module, unique_ptr<LLVMContext> context)
{
    auto tsm = ThreadSafeModule(std::move(module), std::move(context));
    if (auto Err = jit.addIRModule(std::move(tsm))) {
        llvm::errs() << "Failed to add module: " << toString(std::move(Err)) << "\n";
        return 1;
    }

    // main 함수 찾고 호출
    auto mainSym = jit.lookup("main");
    if (!mainSym) {
        llvm::errs() << "main not found: " << toString(mainSym.takeError()) << "\n";
        return 1;
    }

    using MainFn = int();
    auto* Main = (MainFn*)mainSym->getValue();
    return Main();
}

void DeclareFuncs(Module& module, LLVMContext& context)
{
    // declare i32 @printf(ptr, ...)
    auto* ptrTy = PointerType::get(context, 0);
    auto* int32Ty = Type::getInt32Ty(context);

    // 2. 함수 타입 생성: i32 printf(ptr, ...)
    FunctionType* printfTy = FunctionType::get(
        int32Ty,     // 리턴 타입: i32
        {ptrTy},     // 고정 인자: ptr
        true                           // 가변 인자 함수
    );

    // 3. 모듈에 함수 선언 삽입
    FunctionCallee printfFunc = module.getOrInsertFunction("printf", printfTy);
}

void BuildMyFunc(Module& module, LLVMContext& context)
{
    auto* int32Ty = Type::getInt32Ty(context);

    // int -> int 타입으로 만들어 보자
    auto* funcType = FunctionType::get(int32Ty, {int32Ty}, false);
    auto* func = Function::Create(funcType, Function::ExternalLinkage, "my_func", module);

    auto* entryBB = BasicBlock::Create(context, "entry", func);

    auto* arg0 = func->getArg(0);

    IRBuilder<> builder(entryBB);

    auto* added = builder.CreateAdd(arg0, ConstantInt::get(int32Ty, 42));

    builder.CreateRet(added);
}

void BuildMain(Module& module, LLVMContext& context)
{
    auto* int32Ty = Type::getInt32Ty(context);
    auto* funcType = FunctionType::get(int32Ty, false);

    auto* func = Function::Create(funcType, Function::ExternalLinkage, "main", module);

    auto* entryBB = BasicBlock::Create(context, "entry", func);

    auto* my_func = module.getFunction("my_func");

    IRBuilder<> builder(entryBB);

    auto* c = ConstantInt::get(int32Ty, 22);
    auto* result = builder.CreateCall(my_func, {c});

    // print를 어떻게 하나요
    auto* printf_func = module.getFunction("printf");

    auto* text = builder.CreateGlobalString("%d\\n");
    builder.CreateCall(printf_func, {text, result});

    builder.CreateRet(ConstantInt::get(int32Ty, 0));
}

int main(int argc, char* argv[])
{
    // 초기화
    InitLLVM X(argc, argv);
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    
    
    unique_ptr<LLVMContext> context = make_unique<LLVMContext>();
    unique_ptr<Module> module = make_unique<Module>("MyModule", *context);

    DeclareFuncs(*module, *context);
    BuildMyFunc(*module, *context);
    BuildMain(*module, *context);
    
    auto eJit = LLJITBuilder().create();
    if (!eJit) 
    {
        errs() << "Failed to create LLJIT: " << toString(eJit.takeError()) << "\n";
        return 1;
    }

    auto& jit = *eJit;

    auto& jd = jit->getMainJITDylib();

    /*SymbolMap m;

    pair<SymbolStringPtr, ExecutorSymbolDef> p;
    p.first = jit->mangleAndIntern("printf");
    p.second = llvm::JITEvaluatedSymbol((uint64_t)&printf, llvm::JITSymbolFlags::Exported);

    m.insert(p);*/

    auto err = jd.define(llvm::orc::absoluteSymbols({
        {
            jit->mangleAndIntern("printf"),
            llvm::orc::ExecutorSymbolDef(
                llvm::orc::ExecutorAddr::fromPtr(&printf),
                llvm::JITSymbolFlags::Exported)
        }
    }));

    if (err)
    {
    }

    // auto err = jd.define(llvm::orc::absoluteSymbols(m));
    
    int result = Execute(*jit, move(module), move(context));

    std::cout << "main() returned " << result << std::endl;
    return 0;

    std::cout << "Hello World!\n";
}