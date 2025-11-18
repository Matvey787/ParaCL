module;

#include <memory>
#include <ostream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include "global/global.hpp"
#include "parser/parser.hpp"


export module ast_graph_dump;


namespace ParaCL {

void link_nodes(std::ostream& out, const void* lhs, const void* rhs);
void condition_link_type(std::ostream& out, const void* lhs, const void* rhs);
void body_link_type(std::ostream& out, const void* lhs, const void* rhs);
void create_node(std::ostream& out, const void* node, const std::string& label, const std::string& more_settings = "");
void dump_body(std::ostream& out, const void* node, const BlockStmt* body);

void dumpExpr(std::ostream& out, const ParaCL::Expr* expr);
void dumpStmt(std::ostream& out, const ParaCL::Stmt* stmt);


export void ast_dump(const ProgramAST& progAST, const std::string& filename = "dot-out/ast.dot")
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


void dumpExpr(std::ostream& out, const Expr* expr)
{
    if (auto bin = dynamic_cast<const BinExpr*>(expr))
    {
        std::string label;
        switch (bin->op)
        {
            case token_t::ADD:   label = "+"; break;
            case token_t::SUB:   label = "-"; break;
            case token_t::MUL:   label = "*"; break;
            case token_t::DIV:   label = "/"; break;
            case token_t::REM:   label = "%"; break;
            case token_t::ISAB:  label = ">"; break;
            case token_t::ISABE: label = ">="; break;
            case token_t::ISLS:  label = "<"; break;
            case token_t::ISLSE: label = "<="; break;
            case token_t::ISEQ:  label = "=="; break;
            case token_t::ISNE:  label = "!="; break;
            case token_t::AND:   label = "&&"; break;
            case token_t::OR:    label = "||"; break;

            default: builtin_unreachable_wrapper("here we parse only binary operations");
        }
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

    builtin_unreachable_wrapper("we must return in some else-if");
}

void dumpStmt(std::ostream& out, const Stmt* stmt)
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
            case token_t::REMASGN: label += " %= :"; break;
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
        dump_body(out, whileStmt, whileStmt->body.get());

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
    else if (auto condition = dynamic_cast<const ConditionStatement*>(stmt))
    {
        create_node(out, condition, "Condition");

        const auto if_stmt = condition->if_stmt.get(); msg_assert(if_stmt, "in condition we always expect if");
        create_node        (out, if_stmt, "IF");
        link_nodes         (out, stmt, if_stmt);
        dumpExpr           (out, if_stmt->condition.get());
        condition_link_type(out, if_stmt, if_stmt->condition.get());
        dump_body          (out, if_stmt, if_stmt->body.get());

        
        for (auto& elif_stmt: condition->elif_stmts)
        {
            create_node(out, elif_stmt.get(), "ELSE IF");
            link_nodes(out, stmt, elif_stmt.get());
            dumpExpr(out, elif_stmt->condition.get());
            condition_link_type(out, elif_stmt.get(), elif_stmt->condition.get());
            dump_body(out, elif_stmt.get(), elif_stmt->body.get());
        }
        
        const auto else_stmt = condition->else_stmt.get();
        if (not else_stmt) return;

        create_node(out, else_stmt, "ELSE");
        link_nodes (out, stmt, else_stmt);
        dump_body  (out, else_stmt, else_stmt->body.get());

        return;
    }
    builtin_unreachable_wrapper("we must return in some else-if");
}

void link_nodes(std::ostream& out, const void* lhs, const void* rhs)
{
    out << "  \"" << lhs << "\" -> \"" << rhs << "\";\n";
}


void create_node(std::ostream& out, const void* node, const std::string& label, const std::string& more_settings)
{
    out << "  \"" << node << "\" [label=\"" << label << "\"";
    
    if (more_settings == "")
    {
        out << "];\n";
        return;
    }

    out << ", " << more_settings << "];\n";
    
}

void condition_link_type(std::ostream& out, const void* lhs, const void* rhs)
{
    out << "  \"" << lhs << "\" -> \"" << rhs
        << "\" [label=\"cond\", fontcolor=\"gray50\"];\n";
}

void body_link_type(std::ostream& out, const void* lhs, const void* rhs)
{
    out << "  \"" << lhs << "\" -> \"" << rhs
        << "\" [label=\"body\", fontcolor=\"gray50\"];\n";
}

void dump_body(std::ostream& out, const void* node, const BlockStmt* body)
{
    for (auto& s: body->statements)
    {
        dumpStmt(out, s.get());
        body_link_type(out, node, s.get());
    }
}


} /* namespace ParaCL */

