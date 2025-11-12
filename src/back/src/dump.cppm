module;

#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "paraCL_crutch_for_parsery.hpp"

// import paraCL;

export module dump;

std::string ptrToStr(const void* ptr);
void dumpExpr(std::ostream& out, const ParaCL::Expr* expr);
void dumpStmt(std::ostream& out, const ParaCL::Stmt* stmt);


export namespace ParaCL {

void dump(ProgramAST& progAST, const std::string& filename = "imgs/ast.dot")
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

}


std::string ptrToStr(const void* ptr) {
    std::ostringstream oss;
    oss << ptr;
    return oss.str();
}

void dumpExpr(std::ostream& out, const ParaCL::Expr* expr) {
    std::string nodeId = ptrToStr(expr);
    std::string label;

    if (auto bin = dynamic_cast<const ParaCL::BinExpr*>(expr)) {
        label = ParaCL::reverseTokenMap.at(bin->op);
        out << "  \"" << nodeId << "\" [label=\"" << label << "\", style=filled, fillcolor=\"lightyellow\"];\n";

        dumpExpr(out, bin->left.get());
        dumpExpr(out, bin->right.get());

        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(bin->left.get()) << "\";\n";
        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(bin->right.get()) << "\";\n";
    }
    else if (auto num = dynamic_cast<const ParaCL::NumExpr*>(expr)) {
        label = "Num: " + std::to_string(num->value);
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";
    }
    else if (auto var = dynamic_cast<const ParaCL::VarExpr*>(expr)) {
        label = "Var: " + var->name;
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";
    }
    else if (auto in = dynamic_cast<const ParaCL::InputExpr*>(expr)) {
        label = "Input";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";
    }
    else {
        throw std::runtime_error("dump: Unknown expression!");
    }
}

void dumpStmt(std::ostream& out, const ParaCL::Stmt* stmt) {
    std::string nodeId = ptrToStr(stmt);
    std::string label;

    if (auto assign = dynamic_cast<const ParaCL::AssignStmt*>(stmt)) {
        label = "Assign: " + assign->name;
        out << "  \"" << nodeId << "\" [label=\"" << label << "\", style=filled, fillcolor=\"lightblue\"];\n";

        dumpExpr(out, assign->value.get());
        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(assign->value.get()) << "\";\n";
    }
    else if (auto print = dynamic_cast<const ParaCL::PrintStmt*>(stmt)) {
        label = "Print";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";

        dumpExpr(out, print->expr.get());
        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(print->expr.get()) << "\";\n";
    }
    else if (auto whileStmt = dynamic_cast<const ParaCL::WhileStmt*>(stmt)) {
        label = "While";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";

        dumpExpr(out, whileStmt->condition.get());
        out << "  \"" << nodeId << "\" -> \"" << ptrToStr(whileStmt->condition.get())
            << "\" [label=\"cond\", fontcolor=\"gray50\"];\n";

        for (auto& s : whileStmt->body->statements) {
            dumpStmt(out, s.get());
            out << "  \"" << nodeId << "\" -> \"" << ptrToStr(s.get())
                << "\" [label=\"body\", fontcolor=\"gray50\"];\n";
        }
    }
    else if (auto block = dynamic_cast<const ParaCL::BlockStmt*>(stmt)) {
        label = "Block";
        out << "  \"" << nodeId << "\" [label=\"" << label << "\"];\n";

        for (auto& s : block->statements) {
            dumpStmt(out, s.get());
            out << "  \"" << nodeId << "\" -> \"" << ptrToStr(s.get()) << "\";\n";
        }
    }
    else {
        throw std::runtime_error("dump: Unknown statement!");
    }
}

