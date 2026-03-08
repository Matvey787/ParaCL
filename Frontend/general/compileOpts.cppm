module;

#include "projInfo.hpp"
#include "llvm/Support/CommandLine.h"
#include <string>
#include <iostream>
#include <utility>
#include <exception>
#include <sstream>
#include <filesystem>
#include <vector>

export module compileOpts;


llvm::cl::list<std::string> InputFiles(
    llvm::cl::Positional,
    llvm::cl::desc("<input .cl files>"),
    llvm::cl::OneOrMore,
    llvm::cl::value_desc("filename")
);


llvm::cl::list<std::string> OutputPaths(
    "o",
    llvm::cl::desc("Specify output filenames or a directory"),
    llvm::cl::value_desc("paths"),
    llvm::cl::ZeroOrMore
);

llvm::cl::alias OutputAlias(
    "output",
    llvm::cl::desc("Alias for -o"),
    llvm::cl::aliasopt(OutputPaths)
);

llvm::cl::opt<std::string> AstDumpFile(
    "d",
    llvm::cl::desc("Dump AST to .dot file for Graphviz"),
    llvm::cl::value_desc("filename"),
    llvm::cl::init("")
);

llvm::cl::alias AstDumpAlias(
    "ast-dump",
    llvm::cl::desc("Alias for -d"),
    llvm::cl::aliasopt(AstDumpFile)
);

llvm::cl::opt<bool> ShowVersion(
    "v",
    llvm::cl::desc("Show version information"),
    llvm::cl::init(false)
);

llvm::cl::opt<bool> ShowHelp(
    "h",
    llvm::cl::desc("Show this help message"),
    llvm::cl::init(false)
);

const std::string noInputFilesErrorMsg = "No input .cl files provided.\n";

std::string getBriefDescription()
{
    using namespace ParaCL::general::projInfo;
    
    std::stringstream ss;
    ss << "ParaCL — the best language in the world\n";
    ss << "Version: " << ParaCLVersion << "\n";
    ss << "Authors:\n";
    
    for (const auto& author : authors)
    {
        ss << "  - " << author.name << " (" << author.gitLink << ")\n";
    }
    
    return ss.str();
}

export namespace ParaCL::general
{


struct CommandLineData
{
    std::vector<std::filesystem::path> inputFiles;
    std::vector<std::filesystem::path> outputFiles;
};

CommandLineData handleCompileOpts(int argc, char** argv)
{
    std::string briefDescription = getBriefDescription();

    llvm::cl::ParseCommandLineOptions(argc, argv, briefDescription);

    if (ShowHelp)
    {
        llvm::cl::PrintHelpMessage();
        exit(0);
    }

    if (ShowVersion)
    {
        std::cout << "ParaCL version: " << ParaCL::general::projInfo::ParaCLVersion << '\n';
        exit(0);
    }

    if (InputFiles.empty())
    {
        throw std::runtime_error(noInputFilesErrorMsg);
    }

    CommandLineData data;
    for (const auto& f : InputFiles) data.inputFiles.emplace_back(f);

    if (OutputPaths.empty()) 
    {
        data.outputFiles.push_back(InputFiles[0]);
        for (size_t i = 1; i < data.inputFiles.size(); ++i) {
            data.outputFiles.push_back(data.inputFiles[i].stem().string() + ".ast.json");
        }
    }
    else if (OutputPaths.size() == 1 && std::filesystem::is_directory(OutputPaths[0]))
    {
        std::filesystem::path outDir = OutputPaths[0];
        for (const auto& in : data.inputFiles)
        {
            data.outputFiles.push_back(outDir / in.filename().replace_extension(".ast.json"));
        }
    }
    else 
    {
        if (OutputPaths.size() != data.inputFiles.size())
        {
            throw std::runtime_error(
                "It is impossible to determine which output file corresponds to the input file; changes are possible in the future."
            );
        }
        
        for (const auto& f : OutputPaths) data.outputFiles.emplace_back(f);
    }

    return data;
}

} 
