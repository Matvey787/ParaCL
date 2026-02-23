#include <iostream>
#include <stdexcept>
#include <typeinfo>
#include <filesystem>
#include <fstream>

import ast;
import write_ast;

using namespace ::ParaCL::ast::node;

namespace ParaCL::ast::node::visit_overload_set
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
{ std::cout << "Variable{" << v.name() << "} " << std::endl; }
}

namespace ParaCL::ast::node::visit_overload_set
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
void visit(Variable const & v, int& i)
{ std::cout << "Variable{" << v.name() << "} " << ++i << std::endl; }

}


void print(BasicNode const & node)
{ return visit<void>(node); }


void print_and_count(BasicNode const & node, int& i)
{ return visit<void, int&>(node, i); }

int main() try
{
    auto&& n1 = BasicNode::create<Scan, void>(Scan{});
    auto&& n2 = BasicNode::create<Print, void>(Print{});
    auto&& n3 = BasicNode::create<Condition, void>(Condition{});

    print(n1);
    print(n2);
    print(n3);
    std::cout << "here\n";

    auto&& n4 = BasicNode{n3};
    std::cout << "here\n";
    
    print(n4);
    std::cout << "here\n";

    auto&& n5 = BasicNode{std::move(n1)};
    print(n5);

    int i = 0;
    auto&& n12 = BasicNode::create<Scan, void, int&>(Scan{});
    auto&& n22 = BasicNode::create<Print, void, int&>(Print{});
    auto&& n32 = BasicNode::create<Condition, void, int&>(Condition{});

    print_and_count(n12, i);
    print_and_count(n22, i);
    print_and_count(n32, i);

    auto&& n42 = BasicNode{n32};
    print_and_count(n42, i);

    auto&& n52 = BasicNode{std::move(n12)};
    print_and_count(n52, i);

    auto&& n62 = BasicNode::create<Variable, void, int&>(Variable{"some name"});
    print_and_count(n62, i);

    std::filesystem::path p = "test.txt";
    
    std::ofstream ofs(p);

    std::cout << "here\n";
    write::write_node(n62, ofs, 0);

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