#warning "Using version, with stupid options parsing"

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>

import compiler;

int main(int argc, char* argv[]) try
{
    if (argc == 2)
        compiler::compile(argv[1], "a.out");
    else if (argc == 4)
        compiler::compile(argv[1]/* = .ast.json */, /* argv[2] = -o , */ argv[3] /* = executable */);
    else
        throw std::invalid_argument("Usage: " + std::string(argv[0]) + " <source>.ast.json [-o executbale]");

    return 0;
}
catch (std::exception const & e)
{
    std::cerr << "Exception cathched: " << e.what() << "\n";
    return 1;
}
catch (...)
{
    std::cerr << "Undefined exceptions catched.\n";
    return 1;
}

