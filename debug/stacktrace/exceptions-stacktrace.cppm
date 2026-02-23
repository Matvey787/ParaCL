module;

#include <boost/stacktrace.hpp>
#include <boost/stacktrace/stacktrace.hpp>
#include <iostream>

export module exception_stack_trace;

namespace debug
{
namespace stacktrace
{

bool is_main(std::string_view function_name)
{
    return "main" == function_name;
}

std::ostream &operator<<(std::ostream &os, const boost::stacktrace::stacktrace &trace)
{
    for (std::size_t it = 0, ite = trace.size(); it != ite; ++it)
    {
        os << " " << it << "#" << " " << trace[it] << "\n";
        if (is_main(trace[it].name()))
            break;
    }

    return os;
}

export template <bool do_flush = false> void show_exception_stacktrace(std::ostream &os = std::cerr)
{
    auto trace = boost::stacktrace::stacktrace::from_current_exception();
    os << "exception stacktrace:\n" << trace;

    if constexpr (not do_flush)
        return;

    os << std::flush;
};

} /* namespace stacktrace */
} /* namespace debug */
