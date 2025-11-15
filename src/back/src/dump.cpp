#include <memory>
#include <ostream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include "global.hpp"
#include "paraCL_crutch_for_parsery.hpp"
#include <filesystem>

namespace ParaCL {

static void link_nodes(std::ostream& out, const void* lhs, const void* rhs);
static void condition_link_type(std::ostream& out, const void* lhs, const void* rhs);
static void body_link_type(std::ostream& out, const void* lhs, const void* rhs);
static void create_node(std::ostream& out, const void* node, const std::string& label, const std::string& more_settings = "");

static void dumpExpr(std::ostream& out, const ParaCL::Expr* expr);
static void dumpStmt(std::ostream& out, const ParaCL::Stmt* stmt);

void dump(const ProgramAST& progAST, const std::string& filename)
{
    std::filesystem::create_directories("dot-out");
    std::ofstream out(filename);
    if (out.fail())
        throw std::runtime_error("failed open " + filename);

    out << "digraph AST {\n";
    out << "  node [shape=box];\n";

    const void* rootId = (void*) &progAST; /* just for same code style */

    std::string label = "Program";
    create_node(out, rootId, label);

    for (auto& stmt : progAST.statements)
    {
        dumpStmt(out, stmt.get());
        link_nodes(out, rootId, stmt.get());
    }

    out << "}\n";
    out.close();

    std::filesystem::create_directories("../ast-dump");
    std::string dot_cmd = "dot -Tsvg " + filename + " -o ../ast-dump/ast.svg";
    system(dot_cmd.c_str());
}


static void dumpExpr(std::ostream& out, const Expr* expr)
{
    if (auto bin = dynamic_cast<const BinExpr*>(expr))
    {
        std::string label = reverseTokenMap.at(bin->op);
        create_node(out, expr, label, "style=filled, fillcolor=\"lightyellow\"");

        dumpExpr(out, bin->left.get());
        link_nodes(out, expr, bin->left.get());
    
        dumpExpr(out, bin->right.get());
        link_nodes(out, expr, bin->right.get());
        return;
    }
    else if (auto num = dynamic_cast<const NumExpr*>(expr))
    {
        std::string label = "Num: " + std::to_string(num->value);
        create_node(out, expr, label);
        return;
    }
    else if (auto var = dynamic_cast<const VarExpr*>(expr))
    {
        std::string label = "Var: " + var->name;
        create_node(out, expr, label);
        return;
    }
    else if ([[maybe_unused]] auto in = dynamic_cast<const InputExpr*>(expr))
    {
        std::string label = "Input";
        create_node(out, expr, label);
        return;
    }
    else if (auto assign = dynamic_cast<const AssignExpr*>(expr))
    {
        std::string label = "Assign expr: " + assign->name;
        create_node(out, expr, label, "style=filled, fillcolor=\"lightblue\"");

        dumpExpr(out, assign->value.get());
        link_nodes(out, expr, assign->value.get());
        return;
    }

    throw std::runtime_error("dump: Unknown expression!");
}

static void dumpStmt(std::ostream& out, const Stmt* stmt)
{
    if (auto assign = dynamic_cast<const AssignStmt*>(stmt))
    {
        std::string  label = "Assign: " + assign->name + " ";
        create_node(out, stmt, label, "style=filled, fillcolor=\"lightblue\"");

        dumpExpr(out, assign->value.get());
        link_nodes(out, stmt, assign->value.get());
        return;
    }
    else if (auto combined_assign = dynamic_cast<const CombinedAssingStmt*>(stmt))
    {
        std::string label = combined_assign->name;
        switch (combined_assign->op)
        {
            case token_t::ADDASGN: label += " += :"; break;
            case token_t::SUBASGN: label += " -= :"; break;
            case token_t::MULASGN: label += " *= :"; break;
            case token_t::DIVASGN: label += " /= :"; break;
            default: builtin_unreachable_wrapper("no :)");
        }

        create_node(out, stmt, label, "style=filled, fillcolor=\"lightblue\"");

        dumpExpr(out, combined_assign->value.get());
        link_nodes(out, stmt, combined_assign->value.get());
        return;
    }
    else if (auto print = dynamic_cast<const PrintStmt*>(stmt))
    {
        std::string label = "Print";
        create_node(out, stmt, label);

        dumpExpr(out, print->expr.get());
        create_node(out, stmt, label);
        link_nodes(out, stmt, print->expr.get());
        return;
    }
    else if (auto whileStmt = dynamic_cast<const WhileStmt*>(stmt))
    {
        std::string label = "While";
        create_node(out, stmt, label);

        dumpExpr(out, whileStmt->condition.get());
        condition_link_type(out, stmt, whileStmt->condition.get());

        for (auto& s : whileStmt->body->statements) {
            dumpStmt(out, s.get());
            body_link_type(out, stmt, s.get());
        }
        return;
    }
    else if (auto block = dynamic_cast<const BlockStmt*>(stmt))
    {
        std::string label = "Block";
        create_node(out, stmt, label);

        for (auto& s : block->statements) {
            dumpStmt(out, s.get());
            link_nodes(out, stmt, s.get());
        }
        return;
    }
    else if (auto condition = dynamic_cast<const IfStatement*>(stmt))
    {
        std::string label = "IF";
        create_node(out, stmt, label);

        dumpExpr(out, condition->condition.get());
        condition_link_type(out, stmt, condition->condition.get());

        for (auto& s : condition->body->statements)
        {
            dumpStmt(out, s.get());
            body_link_type(out, stmt, s.get());
        }
        return;
    }
    throw std::runtime_error("dump: Unknown statement!");
}

static void link_nodes(std::ostream& out, const void* lhs, const void* rhs)
{
    out << "  \"" << lhs << "\" -> \"" << rhs << "\";\n";
}


static void create_node(std::ostream& out, const void* node, const std::string& label, const std::string& more_settings)
{
    out << "  \"" << node << "\" [label=\"" << label << "\"";
    
    if (more_settings == "")
    {
        out << "];\n";
        return;
    }

    out << ", " << more_settings << "];\n";
    
}

static void condition_link_type(std::ostream& out, const void* lhs, const void* rhs)
{
    out << "  \"" << lhs << "\" -> \"" << rhs
        << "\" [label=\"cond\", fontcolor=\"gray50\"];\n";
}

static void body_link_type(std::ostream& out, const void* lhs, const void* rhs)
{
    out << "  \"" << lhs << "\" -> \"" << rhs
        << "\" [label=\"body\", fontcolor=\"gray50\"];\n";
}


} /* namespace ParaCL */

