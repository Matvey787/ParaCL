#include <filesystem>
#include <vector>
#include <thread>
#include <iostream>

import compileOpts;
import general;
import thelast;

int main(int argc, char** argv) try
{
    ParaCL::general::init_logging();

    auto&& [inputs, outputs] = ParaCL::general::handleCompileOpts(argc, argv);

    for (size_t it = 0, ite = inputs.size(); it != ite; ++it)
    {
        auto&& inputPath = inputs[it];
        auto&& outputPath = outputs[it];
        auto&& program = ParaCL::general::generateAST(inputPath.string());
        auto&& parent = outputPath.parent_path();
        last::write(program, outputPath);
    }

    return 0;
}
catch (const std::exception& e)
{    
    std::cerr << e.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cerr << "Undefied exception catched.\n";
    return 1;
}
