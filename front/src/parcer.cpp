#include "paraCL.hpp"

#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <functional>
#include <iostream>

namespace ParaCL::Parser {

using tokenIt = std::vector<ParaCL::Lexer::tokenData_t>::const_iterator;

inline static void
next(tokenIt& it, token_t expectedToken)
{
    if (it->first != expectedToken)
    {
        std::string errorMessage = "Expected: '" + reverseTokenMap.at(expectedToken) + "'";
        throw std::runtime_error(errorMessage);
    }
    ++it;
}

static std::unique_ptr<Stmt> handleStmt (tokenIt& it);
static std::unique_ptr<Expr> handleExpr (tokenIt& it);

static std::unique_ptr<Expr> COMPARATORS(tokenIt& it);
static std::unique_ptr<Expr> ADD_SUB    (tokenIt& it);
static std::unique_ptr<Expr> MUL_DIV    (tokenIt& it);
static std::unique_ptr<Expr> VAR_NUM    (tokenIt& it);

ProgramAST
createAST(const std::vector<ParaCL::Lexer::tokenData_t>& tokens)
{
    ProgramAST program;
    tokenIt it = tokens.begin();

    while (it->first != token_t::EOT) program.statements.push_back(handleStmt(it));
    
    return program;
}

// ================================ statement chierarchy ================================
static std::unique_ptr<Stmt>
handleStmt(tokenIt& it)
{
    if (it->first == token_t::VAR)
    {
        std::string name = std::get<std::string>(it->second);

        next(it, token_t::VAR);
        next(it, token_t::AS);

        auto value = handleExpr(it);

        next(it, token_t::SC);

        return std::make_unique<AssignStmt>(name, std::move(value));
    }

    if (it->first == token_t::PRINT)
    {
        next(it, token_t::PRINT);

        auto expr = handleExpr(it);

        next(it, token_t::SC);

        return std::make_unique<PrintStmt>(std::move(expr));
    }

    if (it->first == token_t::WH)
    {
        next(it, token_t::WH);
        next(it, token_t::LCIB);
        
        auto condition = handleExpr(it);

        next(it, token_t::RCIB);
        next(it, token_t::LCUB);

        std::vector<std::unique_ptr<Stmt>> body;
        while (it->first != token_t::RCUB) body.push_back(handleStmt(it));

        next(it, token_t::RCUB);

        auto block = std::make_unique<BlockStmt>(std::move(body));

        return std::make_unique<WhileStmt>(std::move(condition), std::move(block));
    }

    throw std::runtime_error("Unknown statement");
}

// ================================ expression chierarchy ================================
static std::unique_ptr<Expr>
handleExpr(tokenIt& it)
{
    return COMPARATORS(it);
}

static std::unique_ptr<Expr>
COMPARATORS(tokenIt& it)
{
    auto left = ADD_SUB(it);

    while (it->first != token_t::EOT && (it->first == token_t::ISAB  ||
                                         it->first == token_t::ISABE ||
                                         it->first == token_t::ISLS  ||
                                         it->first == token_t::ISLSE ))
    {
        token_t op = it->first;
        ++it;
        auto right = ADD_SUB(it);
        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

static std::unique_ptr<Expr>
ADD_SUB(tokenIt& it)
{
    auto left = MUL_DIV(it);

    while (it->first != token_t::EOT && (it->first == token_t::ADD || it->first == token_t::SUB))
    {
        token_t op = it->first;
        ++it;
        auto right = MUL_DIV(it);
        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

static std::unique_ptr<Expr>
MUL_DIV(tokenIt& it)
{
    auto left = VAR_NUM(it);

    while (it->first != token_t::EOT && (it->first == token_t::MUL || it->first == token_t::DIV))
    {
        token_t op = it->first;
        ++it;
        auto right = VAR_NUM(it);
        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

static std::unique_ptr<Expr>
VAR_NUM(tokenIt& it)
{
    if (it->first == token_t::NUM)
    {
        int value = std::get<int>(it->second);

        next(it, token_t::NUM);

        return std::make_unique<NumExpr>(value);
    }

    if (it->first == token_t::VAR)
    {
        std::string name = std::get<std::string>(it->second);
        next(it, token_t::VAR);

        if (it->first == token_t::AS)  // присваивание внутри выражения
        {
            next(it, token_t::AS);
            auto value = handleExpr(it);
            
            return std::make_unique<AssignStmt>(name, std::move(value));
        }

        return std::make_unique<VarExpr>(std::move(name));
    }

    if (it->first == token_t::IN)
    {
        next(it, token_t::IN);

        return std::make_unique<InputExpr>();
    }

    if (it->first == token_t::LCIB)
    {
        next(it, token_t::LCIB);
        
        auto newDepth = handleExpr(it);

        next(it, token_t::RCIB);

        return newDepth;
    }

    throw std::runtime_error("Unexpected token in factor");
}


// ================================ AST dump ================================

inline std::string ptrToStr(const void* ptr) {
    std::ostringstream oss;
    oss << ptr;
    return oss.str();
}

static void dumpExpr(std::ostream& out, const Expr* expr);
static void dumpStmt(std::ostream& out, const Stmt* stmt);

void dump(ProgramAST& progAST, const std::string& filename)
{
    std::ofstream out(filename);
    out << "digraph AST {\n";
    out << "  node [shape=box];\n";

    std::string rootId = "Program";
    out << "  \"" << rootId << "\" [label=\"Program\"];\n";

    for (auto& stmt : progAST.statements)
    {
        dumpStmt(out, stmt.get());
        out << "  \"" << rootId << "\" -> \"" << ptrToStr(stmt.get()) << "\";\n";
    }

    out << "}\n";
    out.close();

    std::string dot_cmd = "dot -Tsvg " + filename + " -o imgs/ast.svg";
    system(dot_cmd.c_str());
}

static void dumpExpr(std::ostream& out, const Expr* expr) {
    std::string nodeId = ptrToStr(expr);
    std::string label;

    if (auto bin = dynamic_cast<const BinExpr*>(expr)) {
        label = reverseTokenMap.at(bin->op);
        out << "  \"" << nodeId << "\" [label=\"" << label << "\", style=filled, fillcolor=\"lightyellow\"];\n";

        auto leftExpr =  dynamic_cast<const Expr*>(bin->left.get());
        auto rightExpr = dynamic_cast<const Expr*>(bin->right.get());

        if (!leftExpr || !rightExpr) {
            throw std::runtime_error("BinExpr children are not Expr");
        }

        dumpExpr(out, leftExpr);
        dumpExpr(out, rightExpr);

        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(bin->left.get()) << "\";\n";
        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(bin->right.get()) << "\";\n";
    }
    else if (auto num = dynamic_cast<const NumExpr*>(expr)) {
        label = "Num: " + std::to_string(num->value);
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";
    }
    else if (auto var = dynamic_cast<const VarExpr*>(expr)) {
        label = "Var: " + var->name;
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";
    }
    else if (auto in = dynamic_cast<const InputExpr*>(expr)) {
        label = "Input";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";
    }
}

static void dumpStmt(std::ostream& out, const Stmt* stmt) {
    std::string nodeId = ptrToStr(stmt);
    std::string label;

    if (auto assign = dynamic_cast<const AssignStmt*>(stmt))
    {
        label = "Assign: " + assign->name;
        out << "  \"" << nodeId << "\" [label=\"" << label << "\", style=filled, fillcolor=\"lightblue\"];\n";

        auto expr = dynamic_cast<const Expr*>(assign->value.get());
        if (!expr) throw std::runtime_error("BinExpr children are not Expr");

        dumpExpr(out, expr);

        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(assign->value.get()) << "\";\n";
    }
    else if (auto print = dynamic_cast<const PrintStmt*>(stmt))
    {
        label = "Print";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";

        auto expr = dynamic_cast<const Expr*>(print->expr.get());
        if (!expr) throw std::runtime_error("BinExpr children are not Expr");

        dumpExpr(out, expr);

        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(print->expr.get()) << "\";\n";
    }
    else if (auto whileStmt = dynamic_cast<const WhileStmt*>(stmt))
    {
        label = "While";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";

        dumpExpr(out, whileStmt->condition.get());

        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(whileStmt->condition.get())
            << "\" [label=\"cond\", fontcolor=\"gray50\"];\n";;

        for (auto& s : whileStmt->body->statements)
        {
            dumpStmt(out, s.get());
            out << "  \"" << nodeId << "\" -> \"" << ptrToStr(s.get())
                << "\" [label=\"body\", fontcolor=\"gray50\"];\n";
        }
    }
    else if (auto block = dynamic_cast<const BlockStmt*>(stmt))
    {
        label = "Block";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";

        for (auto& s : block->statements)
        {
            dumpStmt(out, s.get());
            out << "  \"" << nodeId << "\" -> \"" << ptrToStr(s.get()) << "\";\n";
        }
    }
}

}; // Parser