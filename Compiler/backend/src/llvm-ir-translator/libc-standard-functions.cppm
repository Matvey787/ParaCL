module;

//---------------------------------------------------------------------------------------------------------------

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

//---------------------------------------------------------------------------------------------------------------

export module libc_standart_functions;

//---------------------------------------------------------------------------------------------------------------

namespace compiler::llvm_ir_translator
{

//---------------------------------------------------------------------------------------------------------------

/* TODO: singleton pattern */
export
class LibcStandartFunctions final
{
  private:
    llvm::Module &module_;

    llvm::FunctionType *libc_printf_ty_;
    llvm::FunctionType *libc_scanf_ty_;

    llvm::Function *libc_printf_;
    llvm::Function *libc_scanf_;

  public:
    explicit LibcStandartFunctions(llvm::Module &module, llvm::IRBuilder<> &builder);

    llvm::Function *libc_printf() &;
    llvm::Function *libc_scanf() &;

    const llvm::Function *libc_printf() const &;
    const llvm::Function *libc_scanf() const &;
};

//---------------------------------------------------------------------------------------------------------------

LibcStandartFunctions::LibcStandartFunctions(llvm::Module &module, llvm::IRBuilder<> &builder)
    : module_(module),
      libc_printf_ty_(llvm::FunctionType::get(builder.getInt32Ty(), {builder.getInt8Ty()->getPointerTo()}, true)),
      libc_scanf_ty_(llvm::FunctionType::get(builder.getInt32Ty(), {builder.getInt8Ty()->getPointerTo()}, true)),
      libc_printf_(llvm::Function::Create(libc_printf_ty_, llvm::Function::ExternalLinkage, "printf", module)),
      libc_scanf_(llvm::Function::Create(libc_scanf_ty_, llvm::Function::ExternalLinkage, "scanf", module))
{}

//---------------------------------------------------------------------------------------------------------------

llvm::Function *LibcStandartFunctions::libc_printf() &
{ return libc_printf_; }

//---------------------------------------------------------------------------------------------------------------

llvm::Function *LibcStandartFunctions::libc_scanf() &
{ return libc_scanf_; }

//---------------------------------------------------------------------------------------------------------------

const llvm::Function *LibcStandartFunctions::libc_printf() const &
{ return libc_printf_; }

//---------------------------------------------------------------------------------------------------------------

const llvm::Function *LibcStandartFunctions::libc_scanf() const &
{ return libc_scanf_; }

//---------------------------------------------------------------------------------------------------------------

} /* namespace compiler::llvm_ir_translator */

//---------------------------------------------------------------------------------------------------------------
