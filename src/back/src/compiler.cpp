#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include "paraCL_crutch_for_parsery.hpp"
#include "global//global.hpp"

// import paraCL;
namespace ParaCL {

class SymbolTable {
    struct SymbolData {
        int value;
    };
    std::vector<std::unordered_map<std::string, SymbolData>> scopes;

public:
    void enter()
    {
        scopes.emplace_back();
    }

    void leave()
    {
        scopes.pop_back();
    }

    SymbolData* lookup(const std::string& name)
    {
        for (auto it = scopes.rbegin(), itend = scopes.rend(); it != itend; ++it)
        {
            auto founded = it->find(name);

            if (founded != it->end()) return &(founded->second);
        }
        return nullptr;
    }

    void
    declare(const std::string& name, const int value)
    {
        scopes.back()[name] = {value};
    }
};

static void handleStmt(const Stmt* stmt, SymbolTable& table);
static int  handleExpr(const Expr* expr, SymbolTable& table);
static int  executeBinOp(int leftOp, int rightOp, token_t binOp);
static void executeCombinedAssign(int& operand, int value, token_t combAsgn);
// static int  executeUnOp(int operand, token_t unOp);

void compile(const ProgramAST& progAST)
{
    SymbolTable table;
    table.enter(); // global scope
    for (auto& stmt : progAST.statements)
        handleStmt(stmt.get(), table);

    table.leave();
}
 

static void handleStmt(const Stmt* stmt, SymbolTable& table)
{
    if (auto assign = dynamic_cast<const AssignStmt*>(stmt))
    {
        auto e = dynamic_cast<const Expr*>(assign->value.get());
        if (!e) throw std::runtime_error("BinExpr children are not Expr");

        auto result = handleExpr(e, table);

        table.declare(assign->name, result);
    }
    else if (auto combinedAssingExpr = dynamic_cast<const CombinedAssingStmt*>(stmt))
    {
        auto* varValue = table.lookup(combinedAssingExpr->name);
        if (!varValue)
        {
            std::cerr << "error: '" << combinedAssingExpr->name << "' was not declared in this scope\n"
                      << "paracl: failed with exit code 1";
            exit(EXIT_FAILURE);   
        }

        auto e = dynamic_cast<const Expr*>(combinedAssingExpr->value.get());
        if (!e) throw std::runtime_error("BinExpr children are not Expr");

        auto result = handleExpr(e, table);

        executeCombinedAssign(varValue->value, result, combinedAssingExpr->op);
    }

    else if (auto print = dynamic_cast<const PrintStmt*>(stmt))
    {
        auto e = dynamic_cast<const Expr*>(print->expr.get());
        if (!e) throw std::runtime_error("BinExpr children are not Expr");

        auto result = handleExpr(e, table);
        std::cout << result << '\n'; 
    }
    else if (auto whileStmt = dynamic_cast<const WhileStmt*>(stmt))
    {
        const std::vector<std::unique_ptr<Stmt>>&
        bodyStmts = whileStmt->body->statements;


        while (handleExpr(whileStmt->condition.get(), table))
            for (auto& s : bodyStmts) handleStmt(s.get(), table);

    }
    else if (auto block = dynamic_cast<const BlockStmt*>(stmt))
    {
        const std::vector<std::unique_ptr<Stmt>>& blockStmts = block->statements;

        bool blockIsEmpty = blockStmts.empty();

        if (!blockIsEmpty) table.enter();

        for (auto& s : block->statements)
        {
            handleStmt(s.get(), table);
        }
        if (!blockIsEmpty) table.leave();
    }
    else if (auto condition = dynamic_cast<const IfStatement*>(stmt))
    {
        const bool flag = handleExpr(condition->condition.get(), table);
        if (not flag) return;

        handleStmt(condition->body.get(),table);
    }
}


static int handleExpr(const Expr* expr, SymbolTable& table)
{
    if (auto bin = dynamic_cast<const BinExpr*>(expr))
    {
        auto leftExpr =  dynamic_cast<const Expr*>(bin->left.get());
        auto rightExpr = dynamic_cast<const Expr*>(bin->right.get());

        if (!leftExpr || !rightExpr) throw std::runtime_error("BinExpr children are not Expr");

        int leftResult = handleExpr(leftExpr, table);
        int rightResult = handleExpr(rightExpr, table);

        return executeBinOp(leftResult, rightResult, bin->op);
    }
    else if (auto num = dynamic_cast<const NumExpr*>(expr)) {
        return num->value;
    }
    else if (auto var = dynamic_cast<const VarExpr*>(expr)) {
        auto* varValue = table.lookup(var->name);
        if (!varValue)
        {
            // throw std::runtime_error("Var [ " + var->name + " ] doesn't found in table");
            std::cerr << "error: '" << var->name << "' was not declared in this scope\n"
                      << "paracl: failed with exit code 1";
            exit(EXIT_FAILURE);
        }

        return varValue->value;
    }
    else if ([[maybe_unused]] auto in = dynamic_cast<const InputExpr*>(expr))
    {
        int value = 0;
        std::cin >> value;
        return value;
    }
    else if (auto assignExpr = dynamic_cast<const AssignExpr*>(expr))
    {
        auto e = dynamic_cast<const Expr*>(assignExpr->value.get());
        if (!e) throw std::runtime_error("BinExpr children are not Expr");

        auto result = handleExpr(e, table);

        table.declare(assignExpr->name, result);

        return result;
    }
    else if (auto combinedAssingExpr = dynamic_cast<const CombinedAssingExpr*>(expr))
    {
        auto* varValue = table.lookup(combinedAssingExpr->name);
        if (!varValue)
        {
            std::cerr << "error: '" << combinedAssingExpr->name << "' was not declared in this scope\n"
                      << "paracl: failed with exit code 1";
            exit(EXIT_FAILURE);   
        }

        auto e = dynamic_cast<const Expr*>(combinedAssingExpr->value.get());
        if (!e) throw std::runtime_error("BinExpr children are not Expr");

        auto result = handleExpr(e, table);
        
        executeCombinedAssign(varValue->value, result, combinedAssingExpr->op);

        return varValue->value;
    }

    else
    {
        throw std::runtime_error("Unknown expression class");
    }
}

static int executeBinOp(int leftOp, int rightOp, token_t binOp)
{
    switch (binOp)
    {
    case token_t::ADD:   return leftOp +  rightOp;
    case token_t::SUB:   return leftOp -  rightOp;
    case token_t::MUL:   return leftOp *  rightOp;
    case token_t::DIV:   return leftOp /  rightOp;
    case token_t::ISAB:  return leftOp >  rightOp;
    case token_t::ISABE: return leftOp >= rightOp;
    case token_t::ISLS:  return leftOp <  rightOp;
    case token_t::ISLSE: return leftOp <= rightOp;
    case token_t::ISEQ:  return leftOp == rightOp;
    case token_t::ISNE:  return leftOp != rightOp;
    case token_t::AND:   return leftOp && rightOp;
    case token_t::OR:    return leftOp || rightOp;
    default: builtin_unreachable_wrapper("here we parse onlu binary operation");
    }
    builtin_unreachable_wrapper("we must return in switch");
}

/* TODO: add parsing is unary operations */
// static int executeUnOp(int operand, token_t unOp)
// {
//     switch (unOp)
//     {
//     case token_t::UNPLUS : return +operand;
//     case token_t::UNMINUS: return -operand;
//     case token_t::NOT    : return !operand;
//     default: builtin_unreachable_wrapper("here we parse onlu unary operation");
//     }
//     builtin_unreachable_wrapper("we must return in switch");
// }

static void executeCombinedAssign(int& operand, int value, token_t combAsgn)
{
    switch (combAsgn)
    {
    case token_t::ADDASGN: operand += value; break;
    case token_t::SUBASGN: operand -= value; break;
    case token_t::MULASGN: operand *= value; break;
    case token_t::DIVASGN: operand /= value; break;
    default: builtin_unreachable_wrapper("here we parse only combined assing operations");
    }
    return;
}

} /* namespace ParaCL */
