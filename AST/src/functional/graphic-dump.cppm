module;

#include <filesystem>
#include <fstream>
#include <string_view>
#include <string>
#include <stdexcept>
#include <cstdlib>

export module ast_graph_dump;

export import ast;

namespace last::node
{

using unique_node_id_t = const void*;

export
using dumpable = void(unique_node_id_t, std::ofstream&);

namespace graphic_dump
{

void dump(BasicNode const & node, unique_node_id_t unique_node_id, std::ofstream& os)
{ return visit<void, unique_node_id_t, std::ofstream&>(node, unique_node_id, os); }

template<typename NodeT>
unique_node_id_t get_node_unique_id(NodeT const & node)
{ return static_cast<unique_node_id_t>(std::addressof(node)); }

void create_node(std::ofstream& os, unique_node_id_t node_id, std::string_view label, std::string_view more_settings = "")
{
    os << "\t\"" << node_id << "\" [label=\"" << label << "\"";
    if (!more_settings.empty())
        os << ", " << more_settings;
    os << "];\n";
}

void link_nodes(std::ofstream& os, unique_node_id_t from, unique_node_id_t to, std::string_view label = "")
{
    os << "\t\"" << from << "\" -> \"" << to << "\"";
    if (not label.empty())
        os << " [label=\"" << label << "\", fontcolor=\"gray50\"]";
    os << ";\n";
}

void dump_and_link_with_parent(std::ofstream& os, unique_node_id_t parent, BasicNode const& node, std::string_view label = "")
{
    dump(node, get_node_unique_id(node), os);
    link_nodes(os, parent, get_node_unique_id(node), label);
}

} /* namespace graphic_dump */
} /* namespace last::node */

namespace last
{

export
void dump(AST const & ast, std::filesystem::path const & dot_file, std::filesystem::path const & img_file = "")
{
    constexpr const char dot_extension[] = ".dot";
    if (dot_file.extension() != dot_extension)
        throw std::runtime_error("expect .dot (graphiv extension) file as first argument of 'void dump(AST const&, std::filesystem::path const&, std::filesystem::path const&)");

    auto&& os = std::ofstream{dot_file};

    if (os.fail()) throw std::runtime_error("failed open " + dot_file.string());

    os << "digraph AST {\n";
    os << " \tnode [shape=box];\n";
    os << " \trankdir=TB;\n\n";

    node::graphic_dump::dump(ast.root(), node::graphic_dump::get_node_unique_id(ast.root()), os);

    os << "}\n";
    os.close();

    if (img_file.string().empty()) return;

    std::string dot_cmd = "dot -Tsvg " + dot_file.string() + " -o " + img_file.string();
    std::system(dot_cmd.c_str());
}

} /* namespace last */

namespace last::node::visit_specializations
{

template <>
void visit(Scope const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    graphic_dump::create_node(os, unique_node_id, "Scope", "style=filled, fillcolor=\"lightgray\"");

    for (auto&& arg: node)
        graphic_dump::dump_and_link_with_parent(os, unique_node_id, arg);
}

template <>
void visit(Print const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    graphic_dump::create_node(os, unique_node_id, "Print", "style=filled, fillcolor=\"lightyellow\"");

    for (auto&& arg : node)
        graphic_dump::dump_and_link_with_parent(os, unique_node_id, arg);
}

template <>
void visit([[maybe_unused]] Scan const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    graphic_dump::create_node(os, unique_node_id,
                              "Scan", "style=filled, fillcolor=\"lightgreen\"");
}

template <>
void visit(Variable const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    std::string label = "Variable: " + std::string(node.name());
    graphic_dump::create_node(os, unique_node_id,
                              label, "style=filled, fillcolor=\"lightblue\"");
}

template <>
void visit(NumberLiteral const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    std::string label = "Number: " + std::to_string(node.value());
    graphic_dump::create_node(os, unique_node_id, label);
}

template <>
void visit(StringLiteral const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    std::string label = "String: \\\"" + std::string(node.value()) + "\\\"";
    graphic_dump::create_node(os, unique_node_id, label);
}

template <>
void visit(UnaryOperator const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    std::string op_name;
    switch (node.type())
    {
        case UnaryOperator::MINUS: op_name = "-"; break;
        case UnaryOperator::PLUS:  op_name = "+"; break;
        case UnaryOperator::NOT:   op_name = "not"; break;
    }
    std::string label = "Unary: " + op_name;
    graphic_dump::create_node(os, unique_node_id, label, "style=filled, fillcolor=\"lightyellow\"");

    graphic_dump::dump_and_link_with_parent(os, unique_node_id, node.arg(), "arg");
}

template <>
void visit(BinaryOperator const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    std::string label = "Binary: ";
    switch (node.type())
    {
        case BinaryOperator::AND:     label += "&&"; break;
        case BinaryOperator::OR:      label += "||"; break;
        case BinaryOperator::ADD:     label += "+"; break;
        case BinaryOperator::SUB:     label += "-"; break;
        case BinaryOperator::MUL:     label += "*"; break;
        case BinaryOperator::DIV:     label += "/"; break;
        case BinaryOperator::REM:     label += "%"; break;
        case BinaryOperator::ISAB:    label += ">"; break;
        case BinaryOperator::ISABE:   label += ">="; break;
        case BinaryOperator::ISLS:    label += "<"; break;
        case BinaryOperator::ISLSE:   label += "<="; break;
        case BinaryOperator::ISEQ:    label += "=="; break;
        case BinaryOperator::ISNE:    label += "!="; break;
        case BinaryOperator::ASGN:    label += "="; break;
        case BinaryOperator::ADDASGN: label += "+="; break;
        case BinaryOperator::SUBASGN: label += "-="; break;
        case BinaryOperator::MULASGN: label += "*="; break;
        case BinaryOperator::DIVASGN: label += "/="; break;
        case BinaryOperator::REMASGN: label += "%="; break;
        default:                       label += "??"; break;
    }

    graphic_dump::create_node(os, unique_node_id, label, "style=filled, fillcolor=\"lightyellow\"");

    graphic_dump::dump_and_link_with_parent(os, unique_node_id, node.larg(), "left");
    graphic_dump::dump_and_link_with_parent(os, unique_node_id, node.rarg(), "right");
}

template <>
void visit(While const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    graphic_dump::create_node(os, unique_node_id, "While", "style=filled, fillcolor=\"lightpink\"");

    graphic_dump::dump_and_link_with_parent(os, unique_node_id, node.condition(), "condition");
    graphic_dump::dump_and_link_with_parent(os, unique_node_id, node.body(), "body");
}

template <>
void visit(If const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    graphic_dump::create_node(os, unique_node_id, "If", "style=filled, fillcolor=\"lightpink\"");

    graphic_dump::dump_and_link_with_parent(os, unique_node_id, node.condition(), "condition");
    graphic_dump::dump_and_link_with_parent(os, unique_node_id, node.body(), "body");
}

template <>
void visit(Else const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    graphic_dump::create_node(os, unique_node_id, "Else", "style=filled, fillcolor=\"lightpink\"");
    
    graphic_dump::dump_and_link_with_parent(os, unique_node_id, node.body(), "body");
}

template <>
void visit(Condition const& node, unique_node_id_t unique_node_id, std::ofstream& os)
{
    graphic_dump::create_node(os, unique_node_id, "Condition", "style=filled, fillcolor=\"lightpink\"");

    auto&& first_if = true;

    auto&& ifs = node.get_ifs();

    if (ifs.size() == 0) return;

    graphic_dump::dump_and_link_with_parent(os, unique_node_id, *(ifs.begin()), "if");
    
    for (auto&& it = ifs.begin() + 1, ite = ifs.end(); it != ite; ++it)
        graphic_dump::dump_and_link_with_parent(os, unique_node_id, *it, "else-if");

    if (not node.has_else()) return;
    graphic_dump::dump_and_link_with_parent(os, unique_node_id, node.get_else(), "else");
}

//-----------------------------------------------------------------------------
} /* namespace last::node::visit_specializations */
//-----------------------------------------------------------------------------
