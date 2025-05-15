#pragma warning(disable:4244 4722 4267 4146 4624)

#include <iostream>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>

using namespace llvm;

void DeclareFuncs(Module& module, LLVMContext& context)
{   
    // declare i32 @printf(ptr, ...)
    auto* ptrTy = PointerType::get(context, 0);
    auto* int32Ty = Type::getInt32Ty(context);

    // 2. 함수 타입 생성: i32 printf(ptr, ...)
    FunctionType* printfTy= FunctionType::get(
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

int main()
{
    LLVMContext context{};
    Module module{"MyModule", context};
    
    DeclareFuncs(module, context);
    BuildMyFunc(module, context);
    BuildMain(module, context);

    if (bool hasError = verifyModule(module, &outs()))
    {
        return 1;
    }

    std::error_code ec;
    raw_fd_ostream os("output.ll", ec, sys::fs::OF_None);

    if (ec)
    {

    }
    else
    {
        module.print(os, nullptr);
    }

    // std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
