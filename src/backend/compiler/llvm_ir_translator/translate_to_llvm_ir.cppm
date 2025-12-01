module;

//---------------------------------------------------------------------------------------------------------------

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ToolOutputFile.h>

#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <string>

#include "parser/parser.hpp"

#include "global/global.hpp"
#include "log/log_api.hpp"

//---------------------------------------------------------------------------------------------------------------

export module paracl_llvm_ir_translator;

//---------------------------------------------------------------------------------------------------------------

import options_parser;
import compiler_name_table;
import libc_standart_functions;

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
    CompilerNameTable nametable_;

    LibcStandartFunctions libc_standart_functions_;

    std::filesystem::path ir_file_;
    std::filesystem::path object_file_;

    void generate_main(const ProgramAST &ast);
    void generate(const ProgramAST &ast);
    void generate(const Statement *stmt);

    void generate(const PrintStmt *);
    void generate(const AssignStmt *);
    void generate(const CombinedAssingStmt *);
    void generate(const WhileStmt *);
    void generate(const BlockStmt *);
    void generate(const ConditionStatement *);
    void generate_body(const BlockStmt *);

    llvm::Value *generate(const Expression *);
    llvm::Value *generate(const InputExpr *);
    llvm::Value *generate(const BinExpr *);
    llvm::Value *generate(const UnExpr *);
    llvm::Value *generate(const NumExpr *);
    llvm::Value *generate(const VarExpr *);
    llvm::Value *generate(const AssignExpr *);
    llvm::Value *generate(const CombinedAssingExpr *);

    llvm::Value *convert_to_I1(const Expression *);

    llvm::Value *convert_to_i1(llvm::Value *);

    bool is_true(llvm::Value *);

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
      nametable_(module_, builder_), libc_standart_functions_(module_, builder_),
      ir_file_(program_options.llvm_ir_files[current_source_number]),
      object_file_(program_options.object_files[current_source_number])
{
    LOGINFO("paracl: ir translator: ctor");
    nametable_.new_scope();
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::compile_ir() const
{
    LOGINFO("paracl: ir translator: compiling IR to object file: '{}'", object_file_.string());

    std::string command = "clang -O2 -c " + ir_file_.string() + " -o " + object_file_.string();

    const int sys_result = std::system(command.c_str());

    if (sys_result == EXIT_SUCCESS)
    {
        LOGINFO("paracl: ir translator: successfully compiled object file: '{}'", object_file_.string());
        return;
    }

    throw std::runtime_error("failed create " + object_file_.string() + " from " + ir_file_.string());
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate_ir(const ProgramAST &ast)
{
    LOGINFO("paracl: ir translator: starting IR generation");

    generate_main(ast);

    if (llvm::verifyModule(module_, &llvm::errs()) == EXIT_FAILURE)
    {
        LOGERR("paracl: ir translator: generated module is invalid");
        throw std::runtime_error("ir translator: failed generate module");
    }

    LOGINFO("paracl: ir translator: IR generation completed successfully");
    write_ir_in_file();
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate_main(const ProgramAST &ast)
{
    LOGINFO("paracl: ir translator: generating main function");

    llvm::FunctionType *main_type = llvm::FunctionType::get(builder_.getInt32Ty(), false);
    llvm::Function *main_function = llvm::Function::Create(main_type, llvm::Function::ExternalLinkage, "main", module_);

    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(context_, "entry", main_function);

    builder_.SetInsertPoint(entry_block);

    generate(ast);

    generate_int32_return(0);

    if (llvm::verifyFunction(*main_function, &llvm::errs()) == EXIT_FAILURE)
    {
        LOGERR("paracl: ir translator: main function verification failed");
        throw std::runtime_error("ir translator: main function verification failed");
    }

    LOGINFO("paracl: ir translator: main function verified successfully");
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate(const PrintStmt *print)
{
    LOGINFO("paracl: ir translator: generating print statement");

    std::ostringstream fmt;

    std::vector<llvm::Value *> printf_args(1);

    for (const std::unique_ptr<Expression> &arg : print->args)
    {
        if (const StringConstant *str = dynamic_cast<const StringConstant *>(arg.get()))
        {
            LOGINFO("paracl: ir translator: print string constant: '{}'", str->value);
            fmt << str->value;
        }
        else if (const NumExpr *num = dynamic_cast<const NumExpr *>(arg.get()))
        {
            LOGINFO("paracl: ir translator: print number constant: {}", num->value);
            fmt << std::to_string(num->value);
        }
        else
        {
            LOGINFO("paracl: ir translator: print expression");
            fmt << "%d";
            printf_args.push_back(generate(arg.get()));
        }
    }

    fmt << "\n";

    printf_args[0] = builder_.CreateGlobalStringPtr(fmt.str(), "__printfFormat");

    builder_.CreateCall(libc_standart_functions_.libc_printf(), printf_args);
    LOGINFO("paracl: ir translator: print call generated");
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate(const ProgramAST &ast)
{
    LOGINFO("paracl: ir translator: generating {} statements", ast.statements.size());

    for (const std::unique_ptr<Statement> &stmt : ast.statements)
        generate(stmt.get());
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate(const Statement *stmt)
{
    LOGINFO("paracl: ir translator: generating statement");

    if (auto assign = dynamic_cast<const AssignStmt *>(stmt))
    {
        LOGINFO("paracl: ir translator: statement type: assignment");
        return generate(assign);
    }

    if (auto combined_assign_statement = dynamic_cast<const CombinedAssingStmt *>(stmt))
    {
        LOGINFO("paracl: ir translator: statement type: combined assignment");
        return generate(combined_assign_statement);
    }

    if (auto print_statement = dynamic_cast<const PrintStmt *>(stmt))
    {
        LOGINFO("paracl: ir translator: statement type: print");
        return generate(print_statement);
    }

    if (auto while_stmt = dynamic_cast<const WhileStmt *>(stmt))
    {
        LOGINFO("paracl: ir translator: statement type: while loop");
        return generate(while_stmt);
    }

    if (auto block = dynamic_cast<const BlockStmt *>(stmt))
    {
        LOGINFO("paracl: ir translator: statement type: block");
        return generate(block);
    }

    if (auto condition = dynamic_cast<const ConditionStatement *>(stmt))
    {
        LOGINFO("paracl: ir translator: statement type: condition");
        return generate(condition);
    }

    LOGERR("paracl: ir translator: unknown statement type");
    builtin_unreachable_wrapper("we must return in some else-if");
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate(const WhileStmt *while_stmt)
{
    LOGINFO("paracl: ir translator: generating while loop");

    llvm::Function *current_block = builder_.GetInsertBlock()->getParent();

    llvm::BasicBlock *while_block = llvm::BasicBlock::Create(context_, "while", current_block);
    llvm::BasicBlock *after_while_block = llvm::BasicBlock::Create(context_, "endwhile", current_block);

    LOGINFO("paracl: ir translator: creating while condition branch");
    builder_.CreateCondBr(convert_to_I1(while_stmt->condition.get()), while_block, after_while_block);

    builder_.SetInsertPoint(while_block);
    LOGINFO("paracl: ir translator: generating while loop body");
    generate_body(while_stmt->body.get());

    LOGINFO("paracl: ir translator: creating while loop back branch");
    builder_.CreateCondBr(convert_to_I1(while_stmt->condition.get()), while_block, after_while_block);

    builder_.SetInsertPoint(after_while_block);
    LOGINFO("paracl: ir translator: while loop generation completed");
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate(const ConditionStatement *condition)
{
    LOGINFO("paracl: ir translator: generating condition with {} elif branches{}", condition->elif_stmts.size(),
            condition->else_stmt ? " and else branch" : "");

    llvm::Function *current_block = builder_.GetInsertBlock()->getParent();

    llvm::BasicBlock *if_block = llvm::BasicBlock::Create(context_, "if", current_block);

    const size_t elifs_quant = condition->elif_stmts.size();

    std::vector<llvm::BasicBlock *> elif_blocks;
    std::vector<llvm::BasicBlock *> elif_body_blocks;
    for (size_t it = 0, ite = elifs_quant; it < ite; ++it)
    {
        elif_blocks.push_back(llvm::BasicBlock::Create(context_, "elif_cond", current_block));
        elif_body_blocks.push_back(llvm::BasicBlock::Create(context_, "elif_body", current_block));
    }

    llvm::BasicBlock *else_block =
        condition->else_stmt ? llvm::BasicBlock::Create(context_, "else", current_block) : nullptr;

    llvm::BasicBlock *condition_end = llvm::BasicBlock::Create(context_, "fi", current_block);

    llvm::Value *if_cond = convert_to_I1(condition->if_stmt->condition.get());

    llvm::BasicBlock *first_elif = elif_blocks.empty() ? (else_block ? else_block : condition_end) : elif_blocks[0];

    LOGINFO("paracl: ir translator: creating if condition branch");
    builder_.CreateCondBr(if_cond, if_block, first_elif);

    builder_.SetInsertPoint(if_block);
    LOGINFO("paracl: ir translator: generating if branch body");
    generate_body(condition->if_stmt->body.get());
    builder_.CreateBr(condition_end);

    for (size_t it = 0, ite = elifs_quant; it < ite; ++it)
    {
        builder_.SetInsertPoint(elif_blocks[it]);
        llvm::Value *elif_cond = convert_to_I1(condition->elif_stmts[it]->condition.get());

        llvm::BasicBlock *next_block = (it + 1 < ite) ? elif_blocks[it + 1] : (else_block ? else_block : condition_end);

        LOGINFO("paracl: ir translator: creating elif #{} condition branch", it + 1);
        builder_.CreateCondBr(elif_cond, elif_body_blocks[it], next_block);

        builder_.SetInsertPoint(elif_body_blocks[it]);
        LOGINFO("paracl: ir translator: generating elif #{} body", it + 1);
        generate_body(condition->elif_stmts[it]->body.get());
        builder_.CreateBr(condition_end);
    }

    if (else_block)
    {
        builder_.SetInsertPoint(else_block);
        LOGINFO("paracl: ir translator: generating else branch body");
        generate_body(condition->else_stmt->body.get());
        builder_.CreateBr(condition_end);
    }

    builder_.SetInsertPoint(condition_end);
    LOGINFO("paracl: ir translator: condition generation completed");
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate(const AssignStmt *asgn)
{
    LOGINFO("paracl: ir translator: generating assignment for variable: '{}'", asgn->name);

    if (not dynamic_cast<const InputExpr *>(asgn->value.get()))
    {
        LOGINFO("paracl: ir translator: assignment with expression value");
        llvm::Value *expr = generate(asgn->value.get());
        return nametable_.set_value(asgn->name, expr);
    }

    LOGINFO("paracl: ir translator: assignment with input expression");

    llvm::AllocaInst *var = nametable_.get_variable(asgn->name);

    if (not var)
    {
        LOGINFO("paracl: ir translator: creating new variable for input: '{}'", asgn->name);
        nametable_.set_value(asgn->name, llvm::ConstantInt::get(context_, llvm::APInt(32, 0)));
        var = nametable_.get_variable(asgn->name);
    }

    llvm::Value *fmt = builder_.CreateGlobalStringPtr("%d", "__scanfFormat");
    std::vector<llvm::Value *> scanf_args = {fmt, var};

    LOGINFO("paracl: ir translator: generating scanf call for variable: '{}'", asgn->name);
    builder_.CreateCall(libc_standart_functions_.libc_scanf(), scanf_args);
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate(const BlockStmt *block)
{
    LOGINFO("paracl: ir translator: generating block");

    llvm::Function *current_block = builder_.GetInsertBlock()->getParent();

    llvm::BasicBlock *scope_block = llvm::BasicBlock::Create(context_, "new_scope", current_block);
    llvm::BasicBlock *after_scope = llvm::BasicBlock::Create(context_, "end_scope", current_block);

    LOGINFO("paracl: ir translator: creating branch to block: '{}'", "new_scope");
    builder_.CreateBr(scope_block);
    builder_.SetInsertPoint(scope_block);

    generate_body(block);

    LOGINFO("paracl: ir translator: creating branch to after block: '{}'", "end_scope");
    builder_.CreateBr(after_scope);
    builder_.SetInsertPoint(after_scope);
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate(const CombinedAssingStmt *asgn)
{
    LOGINFO("paracl: ir translator: generating combined assignment for variable: '{}'", asgn->name);

    llvm::Value *expr = generate(asgn->value.get());

    LOGINFO("paracl: ir translator: combined assignment operation type: {}", static_cast<int>(asgn->op()));

    switch (asgn->op())
    {
    case combined_assign_t::ADDASGN:
        expr = builder_.CreateAdd(nametable_.get_variable_value(asgn->name), expr, "__addAsgnResult");
        break;
    case combined_assign_t::SUBASGN:
        expr = builder_.CreateSub(nametable_.get_variable_value(asgn->name), expr, "__subAsgnResult");
        break;
    case combined_assign_t::MULASGN:
        expr = builder_.CreateMul(nametable_.get_variable_value(asgn->name), expr, "__mulAsgnResult");
        break;
    case combined_assign_t::DIVASGN:
        expr = builder_.CreateSDiv(nametable_.get_variable_value(asgn->name), expr, "__divAsgnResult");
        break;
    case combined_assign_t::REMASGN:
        expr = builder_.CreateSRem(nametable_.get_variable_value(asgn->name), expr, "__remAsgnResult");
        break;
    default:
        builtin_unreachable_wrapper("he parsing only combined assign operation");
    }

    nametable_.set_value(asgn->name, expr);
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate_body(const BlockStmt *body)
{
    LOGINFO("paracl: ir translator: generating body with {} statements", body->statements.size());

    /* expect that already SetInsertPoint*/

    nametable_.new_scope();

    for (auto &stmt : body->statements)
        generate(stmt.get());

    nametable_.leave_scope();

    /* expect that after SetInsertPoint*/
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *LLVMIRBuilder::generate(const AssignExpr *asgn)
{
    LOGINFO("paracl: ir translator: generating assignment expression for variable: '{}'", asgn->name);
    if (not dynamic_cast<const InputExpr *>(asgn->value.get()))
    {
        LOGINFO("paracl: ir translator: assignment expression with regular value");
        llvm::Value *expr = generate(asgn->value.get());
        nametable_.set_value(asgn->name, expr);
        return expr;
    }

    LOGINFO("paracl: ir translator: assignment expression with input");

    llvm::AllocaInst *var = nametable_.get_variable(asgn->name);

    if (not var)
    {
        LOGINFO("paracl: ir translator: creating variable for input assignment: '{}'", asgn->name);
        nametable_.set_value(asgn->name, llvm::ConstantInt::get(context_, llvm::APInt(32, 0)));
        var = nametable_.get_variable(asgn->name);
    }

    llvm::Value *fmt = builder_.CreateGlobalStringPtr("%d", "__scanfFormat");
    std::vector<llvm::Value *> scanf_args = {fmt, var};

    LOGINFO("paracl: ir translator: generating scanf call in assignment expression");
    builder_.CreateCall(libc_standart_functions_.libc_scanf(), scanf_args);

    return nametable_.get_variable_value(asgn->name);
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *LLVMIRBuilder::generate(const CombinedAssingExpr *asgn)
{
    LOGINFO("paracl: ir translator: generating combined assignment expression for variable: '{}'", asgn->name);

    llvm::Value *expr = generate(asgn->value.get());

    LOGINFO("paracl: ir translator: combined assignment expression operation type: {}", static_cast<int>(asgn->op()));

    switch (asgn->op())
    {
    case combined_assign_t::ADDASGN:
        expr = builder_.CreateAdd(nametable_.get_variable_value(asgn->name), expr, "__addAsgnResult");
        break;
    case combined_assign_t::SUBASGN:
        expr = builder_.CreateSub(nametable_.get_variable_value(asgn->name), expr, "__subAsgnResult");
        break;
    case combined_assign_t::MULASGN:
        expr = builder_.CreateMul(nametable_.get_variable_value(asgn->name), expr, "__mulAsgnResult");
        break;
    case combined_assign_t::DIVASGN:
        expr = builder_.CreateSDiv(nametable_.get_variable_value(asgn->name), expr, "__divAsgnResult");
        break;
    case combined_assign_t::REMASGN:
        expr = builder_.CreateSRem(nametable_.get_variable_value(asgn->name), expr, "__remAsgnResult");
        break;
    default:
        builtin_unreachable_wrapper("he parsing only combined assign operation");
    }

    nametable_.set_value(asgn->name, expr);

    return expr;
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *LLVMIRBuilder::generate(const Expression *expr)
{
    LOGINFO("paracl: ir translator: generating expression");

    if (auto bin = dynamic_cast<const BinExpr *>(expr))
    {
        LOGINFO("paracl: ir translator: expression type: binary operation");
        return generate(bin);
    }

    if (auto un = dynamic_cast<const UnExpr *>(expr))
    {
        LOGINFO("paracl: ir translator: expression type: unary operation");
        return generate(un);
    }

    if (auto num = dynamic_cast<const NumExpr *>(expr))
    {
        LOGINFO("paracl: ir translator: expression type: number constant: {}", num->value);
        return generate(num);
    }

    if (auto var = dynamic_cast<const VarExpr *>(expr))
    {
        LOGINFO("paracl: ir translator: expression type: variable: '{}'", var->name);
        return nametable_.get_variable_value(var->name);
    }

    if (auto in = dynamic_cast<const InputExpr *>(expr))
    {
        LOGINFO("paracl: ir translator: expression type: input");
        return generate(in);
    }

    if (auto assignExpr = dynamic_cast<const AssignExpr *>(expr))
    {
        LOGINFO("paracl: ir translator: expression type: assignment");
        return generate(assignExpr);
    }

    if (auto combinedAssingExpr = dynamic_cast<const CombinedAssingExpr *>(expr))
    {
        LOGINFO("paracl: ir translator: expression type: combined assignment");
        return generate(combinedAssingExpr);
    }

    LOGERR("paracl: ir translator: unknown expression type");
    builtin_unreachable_wrapper("we must return in some else-if");
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *LLVMIRBuilder::generate(const InputExpr *input)
{
    LOGINFO("paracl: ir translator: generating input expression");

    llvm::AllocaInst *temp_var = builder_.CreateAlloca(builder_.getInt32Ty(), nullptr, "__scanf_tmp_var");
    llvm::Value *fmt = builder_.CreateGlobalStringPtr("%d", "__scanfFormat");
    std::vector<llvm::Value *> scanf_args = {fmt, temp_var};

    LOGINFO("paracl: ir translator: generating scanf call for input expression");
    builder_.CreateCall(libc_standart_functions_.libc_scanf(), scanf_args);

    llvm::Value *result = builder_.CreateLoad(builder_.getInt32Ty(), temp_var, "__scanf_result");
    LOGINFO("paracl: ir translator: input expression completed");
    return result;
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *LLVMIRBuilder::generate(const BinExpr *bin)
{
    LOGINFO("paracl: ir translator: generating binary operation: {}", static_cast<int>(bin->op()));

    llvm::Value *lhs = generate(bin->left.get());
    llvm::Value *rhs = generate(bin->right.get());

    switch (bin->op())
    {
    case binary_op_t::ADD:
        return builder_.CreateAdd(lhs, rhs, "__addResult");
    case binary_op_t::SUB:
        return builder_.CreateSub(lhs, rhs, "__subResult");
    case binary_op_t::MUL:
        return builder_.CreateMul(lhs, rhs, "__mulResult");
    case binary_op_t::DIV:
        return builder_.CreateSDiv(lhs, rhs, "__divResult");
    case binary_op_t::REM:
        return builder_.CreateSRem(lhs, rhs, "__remResult");
    case binary_op_t::AND:
        return builder_.CreateAnd(lhs, rhs, "__andResult");
    case binary_op_t::OR:
        return builder_.CreateOr(lhs, rhs, "__orResult");
    case binary_op_t::ISAB:
    case binary_op_t::ISABE:
    case binary_op_t::ISLS:
    case binary_op_t::ISLSE:
    case binary_op_t::ISEQ:
    case binary_op_t::ISNE: {
        LOGINFO("paracl: ir translator: binary comparison operation");
        llvm::Value *cmp_result;
        switch (bin->op())
        {
        case binary_op_t::ISAB:
            cmp_result = builder_.CreateICmpSGT(lhs, rhs, "__isAbResult");
            break;
        case binary_op_t::ISABE:
            cmp_result = builder_.CreateICmpSGE(lhs, rhs, "__isAbeResult");
            break;
        case binary_op_t::ISLS:
            cmp_result = builder_.CreateICmpSLT(lhs, rhs, "__isLsResult");
            break;
        case binary_op_t::ISLSE:
            cmp_result = builder_.CreateICmpSLE(lhs, rhs, "__isLseResult");
            break;
        case binary_op_t::ISEQ:
            cmp_result = builder_.CreateICmpEQ(lhs, rhs, "__isEqResult");
            break;
        case binary_op_t::ISNE:
            cmp_result = builder_.CreateICmpNE(lhs, rhs, "__isNeResult");
            break;
        default:
            builtin_unreachable_wrapper("here parsing >= type operations");
        }
        return builder_.CreateZExt(cmp_result, builder_.getInt32Ty(), "__cmpResult");
    }

    default:
        LOGERR("paracl: ir translator: unknown binary operation: {}", static_cast<int>(bin->op()));
        builtin_unreachable_wrapper("here parsing only binary operations");
    }
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *LLVMIRBuilder::generate(const UnExpr *un)
{
    LOGINFO("paracl: ir translator: generating unary operation: {}", static_cast<int>(un->op()));

    llvm::Value *val = generate(un->operand.get());
    switch (un->op())
    {
    case unary_op_t::PLUS:
        return val;
    case unary_op_t::NOT: {
        LOGINFO("paracl: ir translator: generating logical NOT operation");
        llvm::Value *zero = llvm::ConstantInt::get(builder_.getInt32Ty(), 0);
        llvm::Value *cmp_result = builder_.CreateICmpEQ(val, zero);
        return builder_.CreateZExt(cmp_result, builder_.getInt32Ty(), "__notResult");
    }
    case unary_op_t::MINUS: {
        LOGINFO("paracl: ir translator: generating unary minus operation");
        llvm::Value *zero = llvm::ConstantInt::get(builder_.getInt32Ty(), 0);
        return builder_.CreateSub(zero, val, "__unminusResult");
    }

    default:
        LOGERR("paracl: ir translator: unknown unary operation: {}", static_cast<int>(un->op()));
        builtin_unreachable_wrapper("here parsing only unary operations");
    }
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *LLVMIRBuilder::generate(const NumExpr *num)
{
    LOGINFO("paracl: ir translator: generating number constant: {}", num->value);
    return llvm::ConstantInt::get(context_, llvm::APInt(32, num->value));
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *LLVMIRBuilder::convert_to_I1(const Expression *expression)
{
    LOGINFO("paracl: ir translator: generating boolean expression for condition");
    return convert_to_i1(generate(expression));
}

//---------------------------------------------------------------------------------------------------------------

llvm::Value *LLVMIRBuilder::convert_to_i1(llvm::Value *value)
{
    LOGINFO("paracl: ir translator: converting value to i1 type");
    llvm::Type *value_type = value->getType();
    llvm::Value *zero = llvm::ConstantInt::get(value_type, 0);
    return builder_.CreateICmpNE(value, zero, "__convertToI1Result");
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::generate_int32_return(uint64_t ret_val)
{
    LOGINFO("paracl: ir translator: generating return statement with value: {}", ret_val);
    llvm::APInt return_value{32, ret_val};
    llvm::Value *ret = llvm::ConstantInt::get(context_, return_value);
    builder_.CreateRet(ret);
}

//---------------------------------------------------------------------------------------------------------------

void LLVMIRBuilder::write_ir_in_file()
{
    LOGINFO("paracl: ir translator: writing IR to file: '{}'", ir_file_.string());

    std::error_code ec;
    llvm::ToolOutputFile out(ir_file_.string(), ec, llvm::sys::fs::OF_None);

    if (ec)
        throw std::runtime_error("failed open: '" + ir_file_.string() + "': " + ec.message());

    module_.print(out.os(), nullptr);
    out.keep();

    LOGINFO("paracl: ir translator: IR successfully written to: {}", ir_file_.string());
}

//---------------------------------------------------------------------------------------------------------------

} /* namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------