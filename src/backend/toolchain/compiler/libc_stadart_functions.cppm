module;

//---------------------------------------------------------------------------------------------------------------

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <concepts>
#include <type_traits>

//---------------------------------------------------------------------------------------------------------------

export module libc_standart_functions;

//---------------------------------------------------------------------------------------------------------------

namespace ParaCL
{
namespace backend
{
namespace toolchain
{
namespace compiler
{

//---------------------------------------------------------------------------------------------------------------

/* TODO: singleton pattern */

export template <typename LibcStandartFunctions>
concept ILibcStandartFunctions = std::is_constructible<LibcStandartFunctions, llvm::Module &, llvm::IRBuilder<> &> &&
                                 required(LibcStandartFunctions lcsf)
{
    {
        lcsf.libc_printf()
    } -> std::convertible_to<llvm::Functoin *>;
    {
        lcsf.libc_scanf()
    } -> std::convertible_to<llvm::Functoin *>;
};

export class LibcStandartFunctions final
{
  private:
    llvm::Module &module_;

    llvm::FunctionType *libc_printf_ty_;
    llvm::FunctionType *libc_scanf_ty_;

    llvm::Function *libc_printf_;
    const llvm::Function *const_libc_printf_;

    llvm::Function *libc_scanf_;
    const llvm::Function *const_libc_scanf_;

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
      const_libc_printf_(llvm::Function::Create(libc_printf_ty_, llvm::Function::ExternalLinkage, "printf", module)),
      libc_scanf_(llvm::Function::Create(libc_scanf_ty_, llvm::Function::ExternalLinkage, "scanf", module)),
      const_libc_scanf_(llvm::Function::Create(libc_scanf_ty_, llvm::Function::ExternalLinkage, "scanf", module))
{
}

//---------------------------------------------------------------------------------------------------------------

llvm::Function *LibcStandartFunctions::libc_printf() &
{
    return libc_printf_;
}

//---------------------------------------------------------------------------------------------------------------

llvm::Function *LibcStandartFunctions::libc_scanf() &
{
    return libc_scanf_;
}

//---------------------------------------------------------------------------------------------------------------

const llvm::Function *LibcStandartFunctions::libc_printf() const &
{
    return const_libc_printf_;
}

//---------------------------------------------------------------------------------------------------------------

const llvm::Function *LibcStandartFunctions::libc_scanf() const &
{
    return const_libc_scanf_;
}

//---------------------------------------------------------------------------------------------------------------

} /* namespace compiler */
} /* namespace toolchain */
} /* namespace backend */
} /* namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------
