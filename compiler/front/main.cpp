#include <iostream>

import ast;

int main()
{
    using namespace ParaCL::front::AST;
    
    // Создаём программу
    ProgramAST prog;
    
    // Добавляем инструкции
    prog.addStatement(make_node<Print>(
        std::vector<AnonNode>{
            make_node<StringLiteral>("Hello, "),
            make_node<StringLiteral>("World!"),
            make_node<NumberLiteral>(42)
        }
    ));
    
    // Создаём переменную
    auto var = make_node<Variable>("x");
    
    // Создаём присваивание
    prog.addStatement(make_node<BinaryOperator>(
        BinaryOperator::BinaryOperatorT::ASGN,
        std::move(var),
        make_node<NumberLiteral>(10)
    ));
    
    return 0;
}
