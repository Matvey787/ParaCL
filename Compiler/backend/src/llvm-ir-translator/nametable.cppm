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

// #include "log/log_api.hpp"
#define LOGINFO(...)
#define LOGERR(...)

//---------------------------------------------------------------------------------------------------------------

export module nametable;

//---------------------------------------------------------------------------------------------------------------

namespace compiler::nametable
{

//---------------------------------------------------------------------------------------------------------------

export class Nametable final
{
  private:
    llvm::Module &module_;
    llvm::IRBuilder<> &builder_;

    std::vector<std::unordered_map<std::string_view, llvm::AllocaInst *>> scopes_;

    llvm::AllocaInst *lookup(std::string_view name);
    void declare(std::string_view name, llvm::Value * = nullptr);

  public:
    Nametable(llvm::Module &module, llvm::IRBuilder<> &builder);

    void new_scope();
    void leave_scope();

    llvm::AllocaInst *get_variable(std::string_view name);
    llvm::Value *get_variable_value(std::string_view name);

    void set_value(std::string_view name, llvm::Value *value);
};

//---------------------------------------------------------------------------------------------------------------

Nametable::Nametable(llvm::Module &module, llvm::IRBuilder<> &builder)
    : module_(module), builder_(builder), scopes_()
{
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::new_scope()
{
    LOGINFO("paracl: compiler: nametable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::leave_scope()
{
    LOGINFO("paracl: compiler: nametable: exiting scope");
    if (scopes_.empty()) return;
    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

llvm::AllocaInst *Nametable::get_variable(std::string_view name)
{
    LOGINFO("paracl: compiler: nametable: searching variable: \"{}\"", name);

    for (auto &&scopes_it : scopes_ | std::views::reverse)
    {
        auto&& found = scopes_it.find(name);
        if (found == scopes_it.end()) continue;
        LOGINFO("paracl: compiler: nametable: variable found: \"{}\"", name);
        return found->second;
    }

    LOGINFO("paracl: compiler: nametable: variable NOT found: \"{}\"", name);
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *Nametable::get_variable_value(std::string_view name)
{
    auto&& var = get_variable(name);

    if (not var) return nullptr;

    return builder_.CreateLoad(builder_.getInt32Ty(), var, std::string(name) + "_load");
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::set_value(std::string_view name, llvm::Value *value)
{
    LOGINFO("paracl: compiler: nametable: set \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    auto&& var = lookup(name);

    if (not var) return declare(name, value);

    builder_.CreateStore(value, var);
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

llvm::AllocaInst *Nametable::lookup(std::string_view name)
{
    for (auto &&scopes_it : scopes_ | std::views::reverse)
    {
        auto&& found = scopes_it.find(name);

        if (found == scopes_it.end())
            continue;

        return found->second;
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::declare(std::string_view name, llvm::Value *value)
{
    LOGINFO("paracl: compiler: nametable: declare \"{}\"", name);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    auto&& var = scopes_.back()[name];
    var = builder_.CreateAlloca(builder_.getInt32Ty(), nullptr, name);

    if (not value) return;

    builder_.CreateStore(value, var);
}

//---------------------------------------------------------------------------------------------------------------

} /* namespace compiler::nametable */

//---------------------------------------------------------------------------------------------------------------
