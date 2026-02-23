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

export module interpreter_options;

//---------------------------------------------------------------------------------------------------------------

export import options;

//---------------------------------------------------------------------------------------------------------------

namespace ParaCL
{
namespace options
{
namespace interpreter
{

//---------------------------------------------------------------------------------------------------------------

using file = std::filesystem::path;

//---------------------------------------------------------------------------------------------------------------

export template <typename InterpreterOptions>
concept IInterpreterOptions =
    requires(InterpreterOptions io, size_t i)
    {
        { io.i_source(i) } -> std::convertible_to<const file &>;
        { io.sources() } -> std::convertible_to<const std::vector<file> &>;
        { io.input_files_quant() } -> std::convertible_to<size_t>;
    };

//---------------------------------------------------------------------------------------------------------------

export template <options::IOptions Options = options::Options> class InterpreterOptions
{
private:
    std::vector<file> sources_;
public:
    explicit InterpreterOptions(const Options &opt) : sources_(opt.sources())
    {}

    const file &i_source(size_t i) const &
    {
        if (i >= sources_.size())
            throw std::runtime_error("try to get no existence source");
        return sources_[i];
    }

    const std::vector<file> &sources() const &
    { return sources_; }

    size_t input_files_quant() const
    { return sources_.size(); }

};

//---------------------------------------------------------------------------------------------------------------

static_assert(IInterpreterOptions<InterpreterOptions<>>, "InterpreterOptions must satisfy IInterpreterOptions concept");

//---------------------------------------------------------------------------------------------------------------

} /* namespace interpreter */
} /* namespace options */
} /* namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------
