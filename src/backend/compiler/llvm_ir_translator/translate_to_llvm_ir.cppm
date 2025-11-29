// !!!!!!!!!!!!!!!!!!!!!!!!!! ПОКА НЕ ГОТОВО !!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!! ПОКА НЕ ГОТОВО !!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!! ПОКА НЕ ГОТОВО !!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!! ПОКА НЕ ГОТОВО !!!!!!!!!!!!!!!!!!!!!!!!!!

module;

//---------------------------------------------------------------------------------------------------------------

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ToolOutputFile.h>

#include <filesystem>
#include <stdexcept>

#include "parser/parser.hpp"

#include "log/log_api.hpp"

//---------------------------------------------------------------------------------------------------------------

export module paracl_llvm_ir_translator;

//---------------------------------------------------------------------------------------------------------------

import options_parser;
import name_table;

//---------------------------------------------------------------------------------------------------------------

namespace ParaCL
{

//---------------------------------------------------------------------------------------------------------------

export class LLVMIRBuilder
{
  private:
    llvm::LLVMContext context_;
    llvm::Module module_;
    llvm::IRBuilder<> builder_;
    NameTable nametable_;

    std::filesystem::path ir_file_;
    std::filesystem::path object_file_;

    void generate_main(const ProgramAST &ast);
    void generate_statements(const ProgramAST &ast);
    void generate_statement(const Statement *stmt);

    void generate_print(const PrintStmt *);
    void generate_assign(const AssignStmt *);
    void generate_combined_assign(const CombinedAssingStmt *);
    void generate_while(const WhileStmt *);
    void generate_block(const BlockStmt *);
    void generate_condition(const ConditionStatement *);

    void generate_expression(const Expression *expr);
    void generate_input_expression(const InputExpr *);
    void generate_binary_op_expression(const BinExpr *);
    void generate_unary_op_expression(const UnExpr *);
    void generate_number_expression(const NumExpr *);
    void generate_variable_expression(const VarExpr *);
    void generate_assign_expression(const AssignExpr *);
    void generate_combined_assign_expression(const CombinedAssingExpr *);

    void generate_int32_return(uint64_t ret_val);

    void write_ir_in_file();

  public:
    LLVMIRBuilder(const Options::program_options_t &program_options, size_t current_source_number);

    void generate_ir(const ProgramAST &ast);
    void compile_ir() const;
};

//---------------------------------------------------------------------------------------------------------------

LLVMIRBuilder::LLVMIRBuilder(const Options::program_options_t &program_options, size_t current_source_number)
    : context_(), module_(program_options.sources[current_source_number].string(), context_), builder_(context_),
      nametable_(), ir_file_(program_options.llvm_ir_files[current_source_number]),
      object_file_(program_options.object_files[current_source_number])
{
    LOGINFO("paracl: ir translator: ctor");
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::compile_ir() const
{
    std::string command = "clang -O2 -c " + ir_file_.string() + " -o " + object_file_.string();

    const int sys_result = std::system(command.c_str());

    if (sys_result == EXIT_SUCCESS)
        return;

    throw std::runtime_error("failed create " + object_file_.string() + " from " + ir_file_.string());
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate_ir(const ProgramAST &ast)
{
    generate_main(ast);

    if (llvm::verifyModule(module_, &llvm::errs()))
    {
        llvm::errs() << "Error: Generated module is invalid!\n";
        return;
    }

    write_ir_in_file();
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate_main(const ProgramAST &ast)
{
    llvm::FunctionType *main_type = llvm::FunctionType::get(builder_.getInt32Ty(), false);
    llvm::Function *main_function = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, "main", module_);

    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(context_, "entry", main_function);

    builder_.SetInsertPoint(entry_block);

    // generate_statements(ast);

    generate_int32_return(0);

    llvm::verifyFunction(*main_function);
}

//---------------------------------------------------------------------------------------------------------------

// void LLVMIRBuilder::generate_statements(const ProgramAST& ast)
// {
//     for (std::unique_ptr<Statement>& stmt: ast.statements)
//         generate_statement(stmt.get());
// }

// //---------------------------------------------------------------------------------------------------------------

// void LLVMIRBuilder::generate_statement(const Statement* stmt)
// {
//     if (auto assign = dynamic_cast<const AssignStmt *>(stmt))
//         return execute_assign_statement(assign);

//     if (auto combined_assign_statement = dynamic_cast<const CombinedAssingStmt *>(stmt))
//         return execute_combined_assign_statement(combined_assign_statement);

//     if (auto print_statement = dynamic_cast<const PrintStmt *>(stmt))
//         return execute_print_statement(print_statement);

//     if (auto while_stmt = dynamic_cast<const WhileStmt *>(stmt))
//         return execute_while_statement(while_stmt);

//     if (auto block = dynamic_cast<const BlockStmt *>(stmt))
//         return execute_block_statement(block);

//     if (auto condition = dynamic_cast<const ConditionStatement *>(stmt))
//         return execute_condition_statement(condition);

//     builtin_unreachable_wrapper("we must return in some else-if");
// }

// //---------------------------------------------------------------------------------------------------------------

// llvm::Value* LLVMIRBuilder::generate_expression(const Expression* expr)
// {
//     // if (auto bin = dynamic_cast<const BinExpr *>(expr))
//     //     return generate_binary_op_expression(bin);

//     // if (auto un = dynamic_cast<const UnExpr *>(expr))
//     //     return generate_unary_op_expression(un);

//     if (auto num = dynamic_cast<const NumExpr *>(expr))
//         return generate_number_expression(num);

//     if (auto var = dynamic_cast<const VarExpr *>(expr))
//         return execute_variable_expression(var);

//     // if (auto in = dynamic_cast<const InputExpr *>(expr))
//     //     return generate_input_expression(in);

//     // if (auto assignExpr = dynamic_cast<const AssignExpr *>(expr))
//     //     return execute_assign_expression(assignExpr);

//     // if (auto combinedAssingExpr = dynamic_cast<const CombinedAssingExpr *>(expr))
//     //     return execute_combined_assign_expression(combinedAssingExpr);

//     builtin_unreachable_wrapper("we must return in some else-if");
// }

// //---------------------------------------------------------------------------------------------------------------

// void LLVMIRBuilder::generate_input_expression(const InputExpr*)
// {

// }

// //---------------------------------------------------------------------------------------------------------------

// void LLVMIRBuilder::generate_binary_op_expression(const BinExpr*)
// {

// }

// //---------------------------------------------------------------------------------------------------------------

// void LLVMIRBuilder::generate_unary_op_expression(const UnExpr*)
// {

// }

// //---------------------------------------------------------------------------------------------------------------

// llvm::Value* LLVMIRBuilder::generate_number_expression(const NumExpr* num)
// {
//     return llvm::ConstantInt::get(context_, llvm::APInt(32, num->value));
// }

// //---------------------------------------------------------------------------------------------------------------

// llvm::Value* LLVMIRBuilder::generate_variable_expression(const VarExpr* var)
// {
//     std::optional<NameValue> varValue = nametable_.get_variable_value(var->name);
//     if (not varValue.has_value())
//         throw std::runtime_error("using undeclarated variable:" + var->name);

//     llvm::AllocaInst* varriale = builder_.CreateAlloca(builder_.getInt32Ty(), nullptr, var->name);

//     llvm::Value* load = builder_.CreateLoad(builder_.getInt32(), varriale, "var_value");
// }

// //---------------------------------------------------------------------------------------------------------------

// void LLVMIRBuilder::generate_assign_expression(const AssignExpr* asgn)
// {
//     llvm::AllocaInst* variable = get_variable(asgn->name);
//     llvm::Value* expression = generate_expression(asgn->value);

//     builder_.CreateStore(expression, varible);
// }

// //---------------------------------------------------------------------------------------------------------------

// void LLVMIRBuilder::generate_combined_assign_expression(const CombinedAssingExpr*)
// {

// }

// //---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate_int32_return(uint64_t ret_val)
{
    llvm::APInt return_value{32, ret_val};
    llvm::Value *ret = llvm::ConstantInt::get(context_, return_value);
    builder_.CreateRet(ret);
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::write_ir_in_file()
{
    LOGINFO("paracl: ir translator: writing IR to file: {}", ir_file_.string());

    std::error_code ec;
    llvm::ToolOutputFile out(ir_file_.string(), ec, llvm::sys::fs::OF_None);

    if (ec)
    {
        llvm::errs() << "failed open: '" << ir_file_.string() << "': " << ec.message() << "\n";
        return;
    }

    module_.print(out.os(), nullptr);
    out.keep();

    LOGINFO("paracl: ir translator: IR successfully written to: {}", ir_file_.string());
}

//---------------------------------------------------------------------------------------------------------------

} /* namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------
