module;

#include <concepts>
#include <filesystem>
#include <type_traits>

export module ast_builder;

import ast;

namespace ParaCL
{
namespace frontend
{
namespace ast_builder
{

using file_t = std::filesystem::path;

export template <typename ASTBuilder>
concept IASTBuilder = std::is_constructible<ASTBuilder, const file_t &> && requires(ASTBuilder ar) {
    { ar.ast() } -> std::same_as<ProramAST>;
};

export class ASTBuilder
{
  public:
    ASTBuilder(const file_t &file);
    ProgramAST ast();
};

} /* namespace ast_builder*/
} /* namespace frontend */
} /* namespace ParaCL */