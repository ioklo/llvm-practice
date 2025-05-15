// 01_Print.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma warning(disable:4244 4722 4267 4146 4624)

#include <iostream>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>


using namespace llvm;

int main()
{
    // StringRef를 쓰는 법을 알아야 한다
    std::string s = "MyModule";

    LLVMContext c{};
    Module m{s, c};

    auto* funcType = FunctionType::get(Type::getInt32Ty(c), false);
    auto* func = Function::Create(funcType, Function::ExternalLinkage, "my_func", m);
    auto* entryBB = BasicBlock::Create(c, "entry", func);

    IRBuilder<> builder(entryBB);
    auto* retValue = ConstantInt::get(Type::getInt32Ty(c), 42);
    builder.CreateRet(retValue);

    m.print(outs(), nullptr);

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
