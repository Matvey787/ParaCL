module;

//---------------------------------------------------------------------------------------------------------------

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "log/log_api.hpp"

//---------------------------------------------------------------------------------------------------------------

export module compiler_nametable;

//---------------------------------------------------------------------------------------------------------------

namespace ParaCL
{
namespace backend
{
namespace toolchain
{
namespace compiler
{

export template <typename Nametable>
concept INametable = std::is_constructible<Nametable> && requires(Nametable nt, std::string_view name, int value) {
    { nt.new_scope() } -> std::same_as<void>;
    { nt.leave_scope() } -> std::same_as<void>;
    { nt.get_variable(name) } -> std::same_as<llvm::AllocaInst *>;
    { nt.get_variable_value(name) } -> std::same_as<llvm::Value *>;
    { nt.set_value(name, value) } -> std::same_as<void>;
};

//---------------------------------------------------------------------------------------------------------------

export class CompilerNameTable final
{
  private:
    llvm::Module &module_;
    llvm::IRBuilder<> &builder_;

    std::vector<std::unordered_map<std::string, llvm::AllocaInst *>> scopes_;

    llvm::AllocaInst *lookup(std::string_view name);
    void declare(std::string_view name, llvm::Value * = nullptr);

  public:
    CompilerNameTable(llvm::Module &module, llvm::IRBuilder<> &builder);

    void new_scope();
    void leave_scope();

    llvm::AllocaInst *get_variable(std::string_view name);
    llvm::Value *get_variable_value(std::string_view name);

    void set_value(std::string_view name, llvm::Value *value);
};

//---------------------------------------------------------------------------------------------------------------

CompilerNameTable::CompilerNameTable(llvm::Module &module, llvm::IRBuilder<> &builder)
    : ICompilerNametable(module, builder), scopes_()
{
}

//---------------------------------------------------------------------------------------------------------------

void CompilerNameTable::new_scope()
{
    LOGINFO("paracl: compiler: nametable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void CompilerNameTable::leave_scope()
{
    LOGINFO("paracl: compiler: nametable: exiting scope");

    if (scopes_.empty())
        return;

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

llvm::AllocaInst *CompilerNameTable::get_variable(std::string_view name)
{
    LOGINFO("paracl: compiler: nametable: searching variable: \"{}\"", name);

    for (auto &scopes_it : scopes_ | std::views::reverse)
    {
        auto found = scopes_it.find(std::string(name));

        if (found == scopes_it.end())
            continue;

        LOGINFO("paracl: compiler: nametable: variable found: \"{}\"", name);
        return found->second;
    }

    LOGINFO("paracl: compiler: nametable: variable NOT found: \"{}\"", name);
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *CompilerNameTable::get_variable_value(std::string_view name)
{
    llvm::AllocaInst *var = get_variable(name);

    if (not var)
        return nullptr;

    return builder_.CreateLoad(builder_.getInt32Ty(), var, std::string(name) + "_load");
}

//---------------------------------------------------------------------------------------------------------------

void CompilerNameTable::set_value(std::string_view name, llvm::Value *value)
{
    LOGINFO("paracl: compiler: nametable: set \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    llvm::AllocaInst *var = lookup(name);

    if (not var)
        return declare(name, value);

    // name_ptr = value;
    builder_.CreateStore(value, var);
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

llvm::AllocaInst *CompilerNameTable::lookup(std::string_view name)
{
    for (auto &scopes_it : scopes_ | std::views::reverse)
    {
        auto found = scopes_it.find(std::string(name));

        if (found == scopes_it.end())
            continue;

        return found->second;
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

void CompilerNameTable::declare(std::string_view name, llvm::Value *value)
{
    LOGINFO("paracl: compiler: nametable: declare \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    llvm::AllocaInst *&var = scopes_.back()[std::string(name)];
    var = builder_.CreateAlloca(builder_.getInt32Ty(), nullptr, name);

    if (not value)
        return;

    builder_.CreateStore(value, var);
}

//---------------------------------------------------------------------------------------------------------------

} /* namespace compiler */
} /* namespace toolchain */
} /* namespace backend */
} /* namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------
