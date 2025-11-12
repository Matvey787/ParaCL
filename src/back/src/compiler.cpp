#include <optional>
#include <limits>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>
#include "paraCL_crutch_for_parsery.hpp"

// import paraCL;

using namespace ParaCL;

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

static void
handleStmt(const Stmt* stmt, SymbolTable& table);
static int
handleExpr(const Expr* expr, SymbolTable& table);
static int
executeBinOp(int leftOp, int rightOp, token_t binOp);

namespace ParaCL {

void
compile(const ParaCL::ProgramAST& progAST)
{
    SymbolTable table;
    table.enter(); // global scope
    for (auto& stmt : progAST.statements)
    {
        handleStmt(stmt.get(), table);
    }
    table.leave();
}
 
}


void
handleStmt(const ParaCL::Stmt* stmt, SymbolTable& table)
{
    if (auto assign = dynamic_cast<const ParaCL::AssignStmt*>(stmt))
    {
        auto e = dynamic_cast<const ParaCL::Expr*>(assign->value.get());
        if (!e) throw std::runtime_error("BinExpr children are not Expr");

        auto result = handleExpr(e, table);

        table.declare(assign->name, result);
    }
    else if (auto print = dynamic_cast<const ParaCL::PrintStmt*>(stmt))
    {
        auto e = dynamic_cast<const ParaCL::Expr*>(print->expr.get());
        if (!e) throw std::runtime_error("BinExpr children are not Expr");

        auto result = handleExpr(e, table);
        std::cout << result << '\n'; 
    }
    else if (auto whileStmt = dynamic_cast<const ParaCL::WhileStmt*>(stmt))
    {
        const std::vector<std::unique_ptr<ParaCL::Stmt>>&
        bodyStmts = whileStmt->body->statements;


        while (handleExpr(whileStmt->condition.get(), table))
            for (auto& s : bodyStmts) handleStmt(s.get(), table);

    }
    else if (auto block = dynamic_cast<const ParaCL::BlockStmt*>(stmt))
    {
        const std::vector<std::unique_ptr<ParaCL::Stmt>>& blockStmts = block->statements;

        bool blockIsEmpty = blockStmts.empty();

        if (!blockIsEmpty) table.enter();

        for (auto& s : block->statements)
        {
            handleStmt(s.get(), table);
        }
        if (!blockIsEmpty) table.leave();
    }
}


int
handleExpr(const ParaCL::Expr* expr, SymbolTable& table)
{
    if (auto bin = dynamic_cast<const ParaCL::BinExpr*>(expr))
    {
        auto leftExpr =  dynamic_cast<const ParaCL::Expr*>(bin->left.get());
        auto rightExpr = dynamic_cast<const ParaCL::Expr*>(bin->right.get());

        if (!leftExpr || !rightExpr) throw std::runtime_error("BinExpr children are not Expr");

        int leftResult = handleExpr(leftExpr, table);
        int rightResult = handleExpr(rightExpr, table);

        return executeBinOp(leftResult, rightResult, bin->op);
    }
    else if (auto num = dynamic_cast<const ParaCL::NumExpr*>(expr)) {
        return num->value;
    }
    else if (auto var = dynamic_cast<const ParaCL::VarExpr*>(expr)) {
        auto* varValue = table.lookup(var->name);
        if (!varValue)
            throw std::runtime_error("Var [ " + var->name + " ] doesn't found in table");

        return varValue->value;
    }
    else if (auto in = dynamic_cast<const ParaCL::InputExpr*>(expr)) {
        int value = 0;
        std::cin >> value;
        return value;
    }
    else if (auto assignExpr = dynamic_cast<const ParaCL::AssignExpr*>(expr))
    {
        auto e = dynamic_cast<const ParaCL::Expr*>(assignExpr->value.get());
        if (!e) throw std::runtime_error("BinExpr children are not Expr");

        auto result = handleExpr(e, table);

        table.declare(assignExpr->name, result);

        return result;
    }
    else
    {
        throw std::runtime_error("Unknown expression class");
    }
}

int
executeBinOp(int leftOp, int rightOp, token_t binOp)
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
    default: throw std::runtime_error("Unknown bin operation");
    }
}

