#include <ios>
#include <iostream>
#include <stdexcept>
#include <typeinfo>
#include <filesystem>
#include <fstream>
#include <utility>
#include <boost/json.hpp>
#include <boost/json/object.hpp>

import thelast;


#include "create-basic-node.hpp"


using namespace ::last::node;
using namespace ::last;

namespace last::node::visit_specializations
{
template <>
void visit([[maybe_unused]] Scan const & scan)
{ std::cout << "Scan" << std::endl; }

template <>
void visit([[maybe_unused]] Print const & print)
{ std::cout << "Print" << std::endl; }

template <>
void visit([[maybe_unused]] Condition const & print)
{ std::cout << "Condition" << std::endl; }

template <>
void visit(Variable const & v)
{ std::cout << "Variable{" << v.name() << "}" << std::endl; }

template <>
void visit(NumberLiteral const & l)
{ std::cout << "NumberLiteral{" << l.value() << "}" << std::endl; }

template <>
void visit(StringLiteral const & l)
{ std::cout << "NumberLiteral{" << l.value() << "}" << std::endl; }

template <>
void visit([[maybe_unused]] If const & l)
{ std::cout << "If{"  << "}" << std::endl; }

template <>
void visit([[maybe_unused]] Else const & l)
{ std::cout << "Else{"  << "}" << std::endl; }

template <>
void visit(Scope const & s)
{ std::cout << "Scope{\n";  for (auto&& m: s) visit<void>(m); std::cout << "}" << std::endl; }

template <>
void visit([[maybe_unused]] BinaryOperator const & bp)
{ std::cout << "BinaryOp{}\n";}

template <>
void visit([[maybe_unused]] UnaryOperator const & uo)
{ std::cout << "UnaryOp{}\n";}

template <>
void visit([[maybe_unused]] While const & uo)
{ std::cout << "While{}\n";}
}

namespace last::node::visit_specializations
{
template <>
void visit([[maybe_unused]] Scan const & scan, int& i)
{ std::cout << "Scan " << ++i << std::endl; }

template <> 
void visit([[maybe_unused]] Print const & print, int& i)
{ std::cout << "Print " << ++i << std::endl; }

template <>
void visit([[maybe_unused]] Condition const & print, int& i)
{ std::cout << "Condition " << ++i << std::endl; }

template <> 
void visit([[maybe_unused]] BinaryOperator const & print, int& i)
{ std::cout << "BinaryOp " << ++i << std::endl; }

template <>
void visit([[maybe_unused]] UnaryOperator const & print, int& i)
{ std::cout << "UnaryOp " << ++i << std::endl; }

template <>
void visit(Variable const & v, int& i)
{ std::cout << "Variable{" << v.name() << "} " << ++i << std::endl; }

template <>
void visit(If const & v, int& i)
{ std::cout << "If{" << "} " << ++i << std::endl; }

template <>
void visit(Else const & v, int& i)
{ std::cout << "Else{" << "} " << ++i << std::endl; }

template <>
void visit(Scope const & v, int& i)
{ std::cout << "Scope{" << "} " << ++i << std::endl; }

template <>
void visit(NumberLiteral const & v, int& i)
{ std::cout << "NumberLiteral{" << "} " << ++i << std::endl; }

template <>
void visit(StringLiteral const & v, int& i)
{ std::cout << "StringLiteral{" << "} " << ++i << std::endl; }

template <>
void visit(While const & v, int& i)
{ std::cout << "While{" << "} " << ++i << std::endl; }

}

using printable = void();
using printable_and_countable = void(int&);

CREATE_SAME(printable, printable_and_countable, writable, dumpable)
#include "read-ast.hpp"

void print(BasicNode const & node)
{ return visit<void>(node); }

void print_and_count(BasicNode const & node, int & i)
{ return visit<void, int&>(node, i); }


int main() try
{
    int i = 0;

    auto&& n = last::node::create(Scan{});
    print(n);

    auto&& n1 = create(Scope{});
    auto&& n2 = create(Scope{});

    print_and_count(n1, i);
    print_and_count(n2, i);

    auto&& n6 = create(NumberLiteral{13});
    auto&& n7 = create(StringLiteral{"fuck me please"});

    auto&& n12 = create(Scan{});
    auto&& n22 = create(Print{});
    auto&& n72 = create(If{{n6}, n22});
    auto&& n82 = create(Else{n12});

    auto&& condition = Condition{{n72}, n82};
    auto&& n32 = create(std::move(condition));
    auto&& condition2 = create(NumberLiteral{42});

    auto&& n8 = create(BinaryOperator{BinaryOperator::ADD, n6, condition2});

    print_and_count(n12, i);
    print(n12);

    print_and_count(n22, i);
    print(n22);

    print_and_count(n32, i);
    print(n32);

    auto&& n42 = BasicNode{n32};
    print_and_count(n42, i);
    print(n42);

    auto&& n52 = BasicNode{std::move(n12)};
    print_and_count(n52, i);
    print(n52);

    auto&& n62 = create(Variable{"some name"});
    print_and_count(n62, i);
    print(n62);

    auto&& nast1 = create(Print{n62, n6, n7});
    auto&& nast2 = n6;
    auto&& nast3 = n32;
    auto&& nast4 = Scope{nast1, nast2, nast3};
    auto&& nast5 = create(Print{n1, n2, n82, n8});
    nast4.push_back(nast5);

    auto&& root = create(std::move(nast4));
    auto&& ast = AST{std::move(root)};
    write(ast, "ast.json");
    dump(ast, "ast.dot", "ast.svg");

    auto&& readed_ast = read("ast.json");
    write(ast, "ast.2.json");
    dump(readed_ast, "ast.2.dot", "readed-ast.svg");

    auto&& name = create(Variable{"artem_lobachev"});
    auto&& num = create(NumberLiteral{999});
    auto&& num2 = create(NumberLiteral{13});
    auto&& binop = create(BinaryOperator{BinaryOperator::ASGN, name, num});
    auto&& print = create(Print{n7, num2, binop});
    auto&& lastastroot = create(Scope{binop, print});
    auto&& lastast = AST{std::move(lastastroot)};
    write(lastast, "last.ast.json");
    dump(lastast, "ast.json.dot", "ast.json.svg");

    std::cout << std::boolalpha << n7.is_a<StringLiteral>() << "\n" << n7.is_a<NumberLiteral>() << "\n";

    StringLiteral back_n7 = static_cast<StringLiteral>(n7);
    std::cout << "back_n7.value() = " << back_n7.value() << "\n";

    NumberLiteral back_n72 = static_cast<NumberLiteral>(n7);
    std::cout << "back_n72.value() = " << back_n72.value() << "\n";

    return 0;
}
catch (const std::runtime_error& e)
{
    std::cerr << "runtime error: " << e.what() << "\n";
    return 1;
}
catch (const std::bad_cast& e)
{
    std::cerr << "bad cast error: " << e.what() << "\n";
    return 1;
}
catch (...)
{
    std::cerr << "excepption catched\n";
    return 1;
}
