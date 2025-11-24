#include "global/custom_console_output.hpp"
module;

//---------------------------------------------------------------------------------------------------------------

#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <stdexcept>

#include "global/global.hpp"

//---------------------------------------------------------------------------------------------------------------

export module name_table;

//---------------------------------------------------------------------------------------------------------------

export namespace ParaCL
{

//---------------------------------------------------------------------------------------------------------------

struct NameValue
{
    int value;
    NameValue() = default;
    explicit NameValue(int value);
};

//---------------------------------------------------------------------------------------------------------------

NameValue::NameValue(int value) :
value(value)
{}

//---------------------------------------------------------------------------------------------------------------

class NameTable
{
private:
    std::vector<std::unordered_map<std::string, NameValue>> scopes_;
    NameValue* lookup(const std::string &name);
    void declare(const std::string &name, int value);

public:
    void enter();
    void leave();
    std::optional<NameValue> get_variable_value(const std::string &name) const;
    void set_value(const std::string &name, const NameValue& value);
};

//---------------------------------------------------------------------------------------------------------------

void NameTable::enter()
{
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void NameTable::leave()
{
    if (scopes_.empty())
        return;

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

std::optional<NameValue> NameTable::get_variable_value(const std::string &name) const
{
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(name);

        if (found == it->end())
            continue;

        return found->second;
    }

    return std::nullopt;
}

//---------------------------------------------------------------------------------------------------------------

void NameTable::set_value(const std::string &name, const NameValue& value)
{
    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    NameValue* name_ptr = lookup(name);

    if (not name_ptr)
        return declare(name, value.value);

    name_ptr->value = value.value;
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

NameValue* NameTable::lookup(const std::string &name)
{
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(name);

        if (found == it->end())
            continue;

        return &(found->second);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

void NameTable::declare(const std::string &name, int value)
{
    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    scopes_.back()[name] = NameValue{value};
}

//---------------------------------------------------------------------------------------------------------------

} /* export namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------