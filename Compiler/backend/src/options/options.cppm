module;

#error "Unsupported now"

#include <cstdlib>
#include <string>
#include <iostream>
#include <utility>
#include <stdexcept>
#include <sstream>
#include <filesystem>

#include <llvm/Support/CommandLine.h>

export module options;

llvm::cl::opt<std::string> AstJsonFile(
    llvm::cl::Positional,
    llvm::cl::desc("<input .ast.json file>"),
    llvm::cl::Required,
    llvm::cl::value_desc("filename")
);

llvm::cl::opt<std::string> OutputFileName(
    "o",
    llvm::cl::desc("Specify output executable filename"),
    llvm::cl::value_desc("filename"),
    llvm::cl::init("a.out")
);

llvm::cl::alias OutputAlias(
    "output",
    llvm::cl::desc("Alias for -o"),
    llvm::cl::aliasopt(OutputFileName)
);

// Уровень оптимизации
llvm::cl::opt<int> OptimizationLevel(
    "O",
    llvm::cl::desc("Optimization level. Choose from:"),
    llvm::cl::value_desc("level"),
    llvm::cl::values(
        clEnumValN(0, "0", "No optimizations (default)"),
        clEnumValN(1, "1", "Basic optimizations"),
        clEnumValN(2, "2", "Aggressive optimizations"),
        clEnumValN(3, "3", "Maximum optimizations")
    ),
    llvm::cl::init(0)
);

// Verbose mode
llvm::cl::opt<bool> VerboseMode(
    "verbose",
    llvm::cl::desc("Show detailed information about compiler actions"),
    llvm::cl::init(false)
);

llvm::cl::alias VerboseAlias(
    "v",
    llvm::cl::desc("Alias for --verbose"),
    llvm::cl::aliasopt(VerboseMode)
);

// Help option
llvm::cl::opt<bool> ShowHelp(
    "help",
    llvm::cl::desc("Show this help message"),
    llvm::cl::init(false)
);

llvm::cl::alias HelpAlias(
    "h",
    llvm::cl::desc("Alias for --help"),
    llvm::cl::aliasopt(ShowHelp)
);

// Version option
llvm::cl::opt<bool> ShowVersion(
    "version",
    llvm::cl::desc("Show version information"),
    llvm::cl::init(false)
);

export namespace compiler::options
{

struct Options
{
    std::filesystem::path astJsonFile;
    std::filesystem::path outputFile;
    int optimizationLevel;
    bool verbose;
};

Options handleCompileOpts(int argc, char** argv)
{
    llvm::cl::ParseCommandLineOptions(argc, argv);

    if (ShowHelp)
    {
        llvm::cl::PrintHelpMessage();
        exit(EXIT_SUCCESS);
    }

    // if (ShowVersion)
    // {
        // /std::cout << "ParaCL compiler version: " << ParaCL::general::projInfo::ParaCLVersion << '\n';
        // exit(EXIT_SUCCESS);
    // }

    auto&& optLevel = OptimizationLevel.getValue();
    if ((optLevel < 0) || (3 < optLevel))
        std::cerr << "ParaCL: warning: optimization level '-O" << optLevel << "' is not supported; using '-O3' instead\n";

    auto&& inputPath  = std::filesystem::path{AstJsonFile   .getValue()};
    auto&& outputPath = std::filesystem::path{OutputFileName.getValue()};

    if (not std::filesystem::exists(inputPath))
        throw std::runtime_error("ParaCL: error: No such file: " + inputPath.string());

    if ((inputPath.extension() != ".ast.json") and (inputPath.extension() != ".json"))
        throw std::runtime_error("ParaCL: error: source file must must have extension '.ast.json'");

    return Options
    {
        .astJsonFile = inputPath,
        .outputFile = outputPath,
        .optimizationLevel = optLevel,
        .verbose = VerboseMode.getValue()
    };
}

} /* namespace compiler::options */
