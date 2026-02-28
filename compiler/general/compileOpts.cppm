module;

#include "llvm/Support/CommandLine.h"
#include <string>

export module parseCompileOpts;

import log;

// ====================== OPTIONS ======================

llvm::cl::opt<std::string> OutputFileName(
    "o",
    llvm::cl::desc("Specify output filename"),
    llvm::cl::value_desc("filename"),
    llvm::cl::init("a.out")
);

llvm::cl::alias OutputAlias(
    "output",
    llvm::cl::desc("Alias for -o"),
    llvm::cl::aliasopt(OutputFileName)
);

// Compile flag
llvm::cl::opt<bool> CompileFlag(
    "c",
    llvm::cl::desc("Compile source file to executable"),
    llvm::cl::init(false)
);

// AST dump to .dot
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

// Version
llvm::cl::opt<bool> ShowVersion(
    "v",
    llvm::cl::desc("Show version information"),
    llvm::cl::init(false)
);

llvm::cl::alias VersionAlias(
    "version",
    llvm::cl::desc("Alias for -v"),
    llvm::cl::aliasopt(ShowVersion)
);

llvm::cl::opt<bool> ShowHelp(
    "h",
    llvm::cl::desc("Show this help message"),
    llvm::cl::init(false)
);

llvm::cl::alias HelpAlias(
    "help",
    llvm::cl::desc("Alias for -h"),
    llvm::cl::aliasopt(ShowHelp)
);

llvm::cl::list<std::string> InputFiles(
    llvm::cl::Positional,
    llvm::cl::desc("<input .cl file>"),
    llvm::cl::ZeroOrMore
);

export namespace ParaCL::general
{

void handleCompileOpts(int argc, char** argv)
{
    llvm::cl::ParseCommandLineOptions(argc, argv, "ParaCL — the best language in the world\n");

    if (ShowHelp) {
        LOGINFO("paracl: options parser: find '--help' flag");
    }
    if (ShowVersion) {
        LOGINFO("paracl: options parser: find '--version' flag");
    }
    if (!AstDumpFile.empty()) {
        LOGINFO("paracl: options parser: find '--ast-dump' = {}", AstDumpFile.getValue());
    }
    if (CompileFlag) {
        LOGINFO("paracl: options parser: find '--compile' flag");
    }
    if (!InputFiles.empty()) {
        LOGINFO("paracl: options parser: input file = {}", InputFiles.front());
    }

    LOGINFO("paracl: options parser: output file = {}", OutputFileName.getValue());
}

} // namespace ParaCL::general