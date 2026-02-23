module;

#include "global/global.hpp"
#include "parser/parser.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

export module ast_graph_dump;

namespace ParaCL
{

void link_nodes(std::ostream &out, const void *lhs, const void *rhs);
void condition_link_type(std::ostream &out, const void *lhs, const void *rhs);
void body_link_type(std::ostream &out, const void *lhs, const void *rhs);
void create_node(std::ostream &out, const void *node, std::string_view label, std::string_view more_settings = "");
void dump_body(std::ostream &out, const void *node, const BlockStmt *body);

void dump(std::ostream &out, const ParaCL::Expression *expr);
void dump(std::ostream &out, const ParaCL::Statement *stmt);
void dump(std::ostream &out, const ParaCL::BlockStmt *block);

export void ast_dump(const ProgramAST &progAST, std::string_view filename)
{
    const std::string file(filename);

    std::ofstream out(file);
    if (out.fail())
        throw std::runtime_error("failed open " + file);

    out << "digraph AST {\n";
    out << "  node [shape=box];\n";

    const void *rootId = (void *)&progAST; /* just for same code style */

    create_node(out, rootId, "Program");

    for (auto &stmt : progAST.statements)
    {
        dump(out, stmt.get());
        link_nodes(out, rootId, stmt.get());
    }

    out << "}\n";
    out.close();

    std::filesystem::create_directories("../ast-dump");
    std::string dot_cmd = "dot -Tsvg " + file + " -o ../ast-dump/ast.svg";
    std::system(dot_cmd.c_str());
}

void dump(std::ostream &out, const Expression *expr)
{
    if (auto bin = static_cast<const BinExpr *>(expr))
    {
        std::string label;
        switch (bin->op())
        {
        case binary_op_t::ADD:
            label = "+";
            break;
        case binary_op_t::SUB:
            label = "-";
            break;
        case binary_op_t::MUL:
            label = "*";
            break;
        case binary_op_t::DIV:
            label = "/";
            break;
        case binary_op_t::REM:
            label = "%";
            break;
        case binary_op_t::ISAB:
            label = ">";
            break;
        case binary_op_t::ISABE:
            label = ">=";
            break;
        case binary_op_t::ISLS:
            label = "<";
            break;
        case binary_op_t::ISLSE:
            label = "<=";
            break;
        case binary_op_t::ISEQ:
            label = "==";
            break;
        case binary_op_t::ISNE:
            label = "!=";
            break;
        case binary_op_t::AND:
            label = "&&";
            break;
        case binary_op_t::OR:
            label = "||";
            break;

        default:
            builtin_unreachable_wrapper("here we parse only binary operations");
        }
        create_node(out, expr, label, "style=filled, fillcolor=\"lightyellow\"");

        dump(out, bin->left.get());
        link_nodes(out, expr, bin->left.get());

        dump(out, bin->right.get());
        link_nodes(out, expr, bin->right.get());
        return;
    }
    else if (auto un = static_cast<const UnExpr *>(expr))
    {
        std::string label;
        switch (un->op())
        {
        case unary_op_t::MINUS:
            label += "-";
            break;
        case unary_op_t::PLUS:
            label += "+";
            break;
        case unary_op_t::NOT:
            label += "not";
            break;
        default:
            builtin_unreachable_wrapper("here we parse onlu unary operation");
        }
        create_node(out, expr, label);
        dump(out, un->operand.get());
        link_nodes(out, expr, un->operand.get());
        return;
    }
    else if (auto num = static_cast<const NumExpr *>(expr))
    {
        std::string label = "Num: " + std::to_string(num->value);
        create_node(out, expr, label);
        return;
    }
    else if (auto var = static_cast<const VarExpr *>(expr))
    {
        std::string label = "Var: " + var->name;
        create_node(out, expr, label);
        return;
    }
    else if ([[maybe_unused]] auto in = static_cast<const InputExpr *>(expr))
    {
        std::string label = "Input";
        create_node(out, expr, label);
        return;
    }
    else if (auto assign = static_cast<const AssignExpr *>(expr))
    {
        std::string label = "Assign expr: " + assign->name;
        create_node(out, expr, label, "style=filled, fillcolor=\"lightblue\"");

        dump(out, assign->value.get());
        link_nodes(out, expr, assign->value.get());
        return;
    }

    else if (auto combined_assign = static_cast<const CombinedAssingExpr *>(expr))
    {
        std::string label = combined_assign->name + " ";
        switch (combined_assign->op())
        {
        case combined_assign_t::ADDASGN:
            label += "+= :";
            break;
        case combined_assign_t::SUBASGN:
            label += "-= :";
            break;
        case combined_assign_t::MULASGN:
            label += "*= :";
            break;
        case combined_assign_t::DIVASGN:
            label += "/= :";
            break;
        case combined_assign_t::REMASGN:
            label += "%= :";
            break;
        default:
            builtin_unreachable_wrapper("here we parse only combined assign");
        }
        create_node(out, expr, label);
        dump(out, combined_assign->value.get());
        link_nodes(out, expr, combined_assign->value.get());
        return;
    }
    else if (auto string = static_cast<const StringConstant *>(expr))
    {
        create_node(out, expr, "STRING: \\\"" + string->value + "\\\"");
        return;
    }

    builtin_unreachable_wrapper("we must return in some else-if");
}

void dump(std::ostream &out, const Statement *stmt)
{
    if (auto assign = static_cast<const AssignStmt *>(stmt))
    {
        std::string label = "Assign: " + assign->name + " ";
        create_node(out, stmt, label, "style=filled, fillcolor=\"lightblue\"");

        dump(out, assign->value.get());
        link_nodes(out, stmt, assign->value.get());
        return;
    }
    else if (auto combined_assign = static_cast<const CombinedAssingStmt *>(stmt))
    {
        std::string label = combined_assign->name;
        switch (combined_assign->op())
        {
        case combined_assign_t::ADDASGN:
            label += " += :";
            break;
        case combined_assign_t::SUBASGN:
            label += " -= :";
            break;
        case combined_assign_t::MULASGN:
            label += " *= :";
            break;
        case combined_assign_t::DIVASGN:
            label += " /= :";
            break;
        case combined_assign_t::REMASGN:
            label += " %= :";
            break;
        default:
            builtin_unreachable_wrapper("no :)");
        }

        create_node(out, stmt, label, "style=filled, fillcolor=\"lightblue\"");

        dump(out, combined_assign->value.get());
        link_nodes(out, stmt, combined_assign->value.get());
        return;
    }
    else if (auto print = static_cast<const PrintStmt *>(stmt))
    {
        std::string label = "Print";
        create_node(out, stmt, label);

        for (auto &arg : print->args)
        {
            dump(out, arg.get());
            link_nodes(out, print, arg.get());
            continue;
        }
        return;
    }
    else if (auto whileStmt = static_cast<const WhileStmt *>(stmt))
    {
        std::string label = "While";
        create_node(out, stmt, label);

        dump(out, whileStmt->condition.get());
        condition_link_type(out, stmt, whileStmt->condition.get());
        dump_body(out, whileStmt, whileStmt->body.get());

        return;
    }
    else if (auto block = static_cast<const BlockStmt *>(stmt))
    {
        return dump(out, block);
    }
    else if (auto condition = static_cast<const ConditionStatement *>(stmt))
    {
        create_node(out, condition, "Condition");

        const auto if_stmt = condition->if_stmt.get();
        msg_assert(if_stmt, "in condition we always expect if");
        create_node(out, if_stmt, "IF");
        link_nodes(out, stmt, if_stmt);
        dump(out, if_stmt->condition.get());
        condition_link_type(out, if_stmt, if_stmt->condition.get());
        dump_body(out, if_stmt, if_stmt->body.get());

        for (auto &elif_stmt : condition->elif_stmts)
        {
            create_node(out, elif_stmt.get(), "ELSE IF");
            link_nodes(out, stmt, elif_stmt.get());
            dump(out, elif_stmt->condition.get());
            condition_link_type(out, elif_stmt.get(), elif_stmt->condition.get());
            dump_body(out, elif_stmt.get(), elif_stmt->body.get());
        }

        const auto else_stmt = condition->else_stmt.get();
        if (not else_stmt)
            return;

        create_node(out, else_stmt, "ELSE");
        link_nodes(out, stmt, else_stmt);
        dump_body(out, else_stmt, else_stmt->body.get());

        return;
    }
    builtin_unreachable_wrapper("we must return in some else-if");
}

void dump(std::ostream &out, const ParaCL::BlockStmt *block)
{
    std::string label = "Block";
    create_node(out, block, label);

    for (auto &s : block->statements)
    {
        dump(out, s.get());
        link_nodes(out, block, s.get());
    }
}

void link_nodes(std::ostream &out, const void *lhs, const void *rhs)
{
    out << "  \"" << lhs << "\" -> \"" << rhs << "\";\n";
}

void create_node(std::ostream &out, const void *node, std::string_view label, std::string_view more_settings)
{
    out << "  \"" << node << "\" [label=\"" << label << "\"";

    if (more_settings == "")
    {
        out << "];\n";
        return;
    }

    out << ", " << more_settings << "];\n";
}

void condition_link_type(std::ostream &out, const void *lhs, const void *rhs)
{
    out << "  \"" << lhs << "\" -> \"" << rhs << "\" [label=\"cond\", fontcolor=\"gray50\"];\n";
}

void body_link_type(std::ostream &out, const void *lhs, const void *rhs)
{
    out << "  \"" << lhs << "\" -> \"" << rhs << "\" [label=\"body\", fontcolor=\"gray50\"];\n";
}

void dump_body(std::ostream &out, const void *node, const BlockStmt *body)
{

    dump(out, body);
    body_link_type(out, node, body);
}

} /* namespace ParaCL */
