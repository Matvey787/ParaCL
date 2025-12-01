module;

//---------------------------------------------------------------------------------------------------------------

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "global/global.hpp"
#include "log/log_api.hpp"

//---------------------------------------------------------------------------------------------------------------

export module interpreter_name_table;

//---------------------------------------------------------------------------------------------------------------

export namespace ParaCL
{

//---------------------------------------------------------------------------------------------------------------

struct NameValue
{
  private:
    int value_;

  public:
    NameValue() = default;
    explicit NameValue(int value);
    int value() const
    {
        return value_;
    }
};

//---------------------------------------------------------------------------------------------------------------

NameValue::NameValue(int value) : value_(value)
{
}

//---------------------------------------------------------------------------------------------------------------

class InterpreterNameTable
{
  private:
    std::vector<std::unordered_map<std::string, NameValue>> scopes_;
    NameValue *lookup(std::string_view name);
    void declare(std::string_view name, int value);

  public:
    void new_scope();
    void leave_scope();
    std::optional<NameValue> get_variable_value(std::string_view name) const;
    void set_value(std::string_view name, const NameValue &value);
};

//---------------------------------------------------------------------------------------------------------------

void InterpreterNameTable::new_scope()
{
    LOGINFO("paracl: interpreter: nametable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void InterpreterNameTable::leave_scope()
{
    LOGINFO("paracl: interpreter: nametable: exiting scope");

    if (scopes_.empty())
        return;

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

std::optional<NameValue> InterpreterNameTable::get_variable_value(std::string_view name) const
{
    LOGINFO("paracl: interpreter: nametable: searching variable: \"{}\"", name);

    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(std::string(name));

        if (found == it->end())
            continue;

        LOGINFO("paracl: interpreter: nametable: variable found: \"{}\" = {}", name, found->second.value());
        return found->second;
    }

    LOGINFO("paracl: interpreter: nametable: variable NOT found: \"{}\"", name);
    return std::nullopt;
}

//---------------------------------------------------------------------------------------------------------------

void InterpreterNameTable::set_value(std::string_view name, const NameValue &value)
{
    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value.value(), name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    NameValue *name_ptr = lookup(name);

    if (not name_ptr)
        return declare(name, value.value());

    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value.value(), name);
    *name_ptr = value;
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

NameValue *InterpreterNameTable::lookup(std::string_view name)
{
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(std::string(name));

        if (found == it->end())
            continue;

        return &(found->second);
    }

    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------

void InterpreterNameTable::declare(std::string_view name, int value)
{
    LOGINFO("paracl: interpreter: nametable: declate {} = \"{}\"", name, value);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    scopes_.back()[std::string(name)] = NameValue{value};
}

//---------------------------------------------------------------------------------------------------------------

} // namespace ParaCL

//---------------------------------------------------------------------------------------------------------------
