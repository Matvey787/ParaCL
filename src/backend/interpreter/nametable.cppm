module;

//---------------------------------------------------------------------------------------------------------------

#include <concepts>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "log/log_api.hpp"

//---------------------------------------------------------------------------------------------------------------

export module interpreter_name_table;

//---------------------------------------------------------------------------------------------------------------

namespace ParaCL
{
namespace backend
{
namespace interpreter
{
namespace nametable
{

//---------------------------------------------------------------------------------------------------------------

export template <typename Nametable>
concept INametable = std::is_constructible<Nametable> && requires(Nametable nt, std::string_view name, int value) {
    { nt.new_scope() } -> std::same_as<void>;
    { nt.leave_scope() } -> std::same_as<void>;
    { nt.get_variable_value(name) } -> std::same_as<std::optional<int>>;
    { nt.set_value(name, value) } -> std::same_as<void>;
};

class Nametable final
{
  private:
    std::vector<std::unordered_map<std::string_view, int>> scopes_;
    int *lookup(std::string_view name);
    void declare(std::string_view name, int value);

  public:
    void new_scope();
    void leave_scope();
    std::optional<int> get_variable_value(std::string_view name) const;
    void set_value(std::string_view name, int value);
};

static_assert(< INametable<Nametable>, "class Nametable must realized INametable interface,"
                                       "because now it`s the only one realizatoin of this interaface,"
                                       "and it`s using in other modules");

//---------------------------------------------------------------------------------------------------------------

void Nametable::new_scope()
{
    LOGINFO("paracl: interpreter: nametable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::leave_scope()
{
    LOGINFO("paracl: interpreter: nametable: exiting scope");

    if (scopes_.empty())
        return;

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

std::optional<int> Nametable::get_variable_value(std::string_view name) const
{
    LOGINFO("paracl: interpreter: nametable: searching variable: \"{}\"", name);

    for (const auto &scopes_it : scopes_ | std::views::reverse)
    {
        auto found = scopes_it.find(name);

        if (found == scopes_it.end())
            continue;

        LOGINFO("paracl: interpreter: nametable: variable found: \"{}\" = {}", name, found->second);
        return found->second;
    }

    LOGINFO("paracl: interpreter: nametable: variable NOT found: \"{}\"", name);
    return std::nullopt;
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::set_value(std::string_view name, int value)
{
    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value, name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    int *name_ptr = lookup(name);

    if (not name_ptr)
        return declare(name, value);

    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value, name);
    *name_ptr = value;
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

int *Nametable::lookup(std::string_view name)
{
    for (auto &scopes_it : scopes_ | std::views::reverse)
    {
        auto found = scopes_it.find(name);

        if (found == scopes_it.end())
            continue;

        return &(found->second);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::declare(std::string_view name, int value)
{
    LOGINFO("paracl: interpreter: nametable: declate {} = \"{}\"", name, value);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    scopes_.back()[name] = value;
}

//---------------------------------------------------------------------------------------------------------------

} /* namespace nametable */
} /* namespace interpreter */
} /* namespace backend */
} /* namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------
