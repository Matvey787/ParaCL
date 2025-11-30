module;

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

export module libc_standart_functions;

namespace ParaCL
{

export struct LibcStandartFunctions
{
  private:
    llvm::Module &module_;
    llvm::IRBuilder<> &builder_;

    llvm::FunctionType *libc_printf_ty_;
    llvm::FunctionType *libc_scanf_ty_;

    llvm::Function *libc_printf_;
    llvm::Function *libc_scanf_;

  public:
    LibcStandartFunctions(llvm::Module &module, llvm::IRBuilder<> &builder);
    llvm::Function *libc_printf();
    llvm::Function *libc_scanf();
};

LibcStandartFunctions::LibcStandartFunctions(llvm::Module &module, llvm::IRBuilder<> &builder)
    : module_(module), builder_(builder),
      libc_printf_ty_(llvm::FunctionType::get(builder_.getInt32Ty(), {builder_.getInt8Ty()->getPointerTo()}, true)),
      libc_scanf_ty_(llvm::FunctionType::get(builder_.getInt32Ty(), {builder_.getInt8Ty()->getPointerTo()}, true)),
      libc_printf_(llvm::Function::Create(libc_printf_ty_, llvm::Function::ExternalLinkage, "printf", module_)),
      libc_scanf_(llvm::Function::Create(libc_scanf_ty_, llvm::Function::ExternalLinkage, "scanf", module_))
{
}

llvm::Function *LibcStandartFunctions::libc_printf()
{
    return libc_printf_;
}

llvm::Function *LibcStandartFunctions::libc_scanf()
{
    return libc_scanf_;
}

} /* namespace ParaCL */
