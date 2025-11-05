#include "paraCL.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

int main(int argc, const char* argv[]) try
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }

    std::ifstream programFile(argv[1]);
    if (!programFile)
    {
        std::cerr << "Cannot open file: " << argv[1] << "\n";
        return 1;
    }

    std::ostringstream buffer;
    buffer << programFile.rdbuf();
    std::string source = buffer.str();

    std::vector<ParaCL::Lexer::tokenData_t> tokens = ParaCL::Lexer::tokenize(source);

    // ParaCL::Lexer::dump(tokens);

    ParaCL::Parser::ProgramAST progAST = ParaCL::Parser::createAST(tokens);

    // ParaCL::Parser::dump(progAST);

    ParaCL::Compiler::compileByCpp(progAST);

}
catch(const std::exception& e)
{
    std::cerr << e.what() << '\n';
}
