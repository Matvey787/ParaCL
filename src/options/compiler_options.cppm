module;

//---------------------------------------------------------------------------------------------------------------

#include <concepts>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

//---------------------------------------------------------------------------------------------------------------

#include "global/global.hpp"

//---------------------------------------------------------------------------------------------------------------

export module compiler_options;

//---------------------------------------------------------------------------------------------------------------

export import options;

//---------------------------------------------------------------------------------------------------------------

namespace ParaCL
{
namespace options
{
namespace compiler
{

//---------------------------------------------------------------------------------------------------------------

using file = std::filesystem::path;

//---------------------------------------------------------------------------------------------------------------

export template <typename CompilerOptions>
concept ICompilerOptions =
    requires(const CompilerOptions &co, size_t i)
    {
        { co.i_source(i) } -> std::convertible_to<const file &>;
        { co.i_object(i) } -> std::convertible_to<const file &>;
        { co.sources() } -> std::convertible_to<const std::vector<file> &>;
        { co.objects() } -> std::convertible_to<const std::vector<file> &>;
        { co.executable() } -> std::convertible_to<const file &>;
        { co.optimize_level() } -> std::same_as<options::optimize_level_t>;
        { co.input_files_quant() } -> std::convertible_to<size_t>;
        { co.compile() } -> std::convertible_to<bool>;
        ON_GRAPHVIZ(
        { co.ast_dump() } -> std::convertible_to<bool>; { co.dot_file() } -> std::convertible_to<const file &>;
        ) /* ON_GRAPHVIZ */
};

//---------------------------------------------------------------------------------------------------------------

export template <options::IOptions Options = options::Options> class CompilerOptions
{
  private:
    std::vector<file> sources_;
    std::vector<file> objects_;
    file executable_file_;
    options::optimize_level_t optimize_level_;
    bool compile_ : 1;

    ON_GRAPHVIZ(bool ast_dump_ : 1; file dot_file_;)

  public:
    explicit CompilerOptions(const Options &opt)
        : sources_(opt.sources()), objects_(opt.objects()), executable_file_(opt.executable()),
          optimize_level_(opt.optimize_level()),
          compile_(opt.compile()) ON_GRAPHVIZ(, ast_dump(opt.ast_dump_), dot_file_(opt.dot_file_))
    {
    }

  public:
    const file &i_source(size_t i) const &
    {
        if (i >= sources_.size())
            throw std::runtime_error("try to get no existence source");
        return sources_[i];
    }

    const file &i_object(size_t i) const &
    {
        if (i >= objects_.size())
            throw std::runtime_error("try to get no existence object");
        return objects_[i];
    }

    const std::vector<file> &sources() const &
    { return sources_; }
    const std::vector<file> &objects() const &
    { return objects_; }

    const file &executable() const &
    { return executable_file_; }

    options::optimize_level_t optimize_level() const
    { return optimize_level_; }

    size_t input_files_quant() const
    { return sources_.size(); }

    bool compile() const
    { return compile_; }

    ON_GRAPHVIZ(
    bool ast_dump() const
    { return ast_dump_; }
    const file &dot_file() const &
    { return dot_file_; }
    )
};

//---------------------------------------------------------------------------------------------------------------

static_assert(ICompilerOptions<CompilerOptions<>>, "CompilerOptions must satisfy ICompilerOptions concept");

//---------------------------------------------------------------------------------------------------------------

} /* namespace compiler*/
} /* namespace options */
} /* namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------
