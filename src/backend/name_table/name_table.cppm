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

export module name_table;

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

class NameTable
{
  private:
    std::vector<std::unordered_map<std::string, NameValue>> scopes_;
    NameValue *lookup(const std::string &name);
    void declare(const std::string &name, int value);

  public:
    void new_scope();
    void leave_scope();
    std::optional<NameValue> get_variable_value(const std::string &name) const;
    void set_value(const std::string &name, const NameValue &value);
};

//---------------------------------------------------------------------------------------------------------------

void NameTable::new_scope()
{
    LOGINFO("paracl: interpreter: nametable: create next scope");
    scopes_.emplace_back();
}

//---------------------------------------------------------------------------------------------------------------

void NameTable::leave_scope()
{
    LOGINFO("paracl: interpreter: nametable: exiting scope");

    if (scopes_.empty())
        return;

    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

std::optional<NameValue> NameTable::get_variable_value(const std::string &name) const
{
    LOGINFO("paracl: interpreter: nametable: searching variable: \"{}\"", name);

    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(name);

        if (found == it->end())
            continue;

        LOGINFO("paracl: interpreter: nametable: variable found: \"{}\" = {}", name, found->second.value());
        return found->second;
    }

    LOGINFO("paracl: interpreter: nametable: variable NOT found: \"{}\"", name);
    return std::nullopt;
}

//---------------------------------------------------------------------------------------------------------------

void NameTable::set_value(const std::string &name, const NameValue &value)
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

NameValue *NameTable::lookup(const std::string &name)
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
    LOGINFO("paracl: interpreter: nametable: declate {} = \"{}\"", name, value);

    if (scopes_.empty())
        throw std::runtime_error("cannot declare variable: no active scopes");

    scopes_.back()[name] = NameValue{value};
}

//---------------------------------------------------------------------------------------------------------------

} // namespace ParaCL

//---------------------------------------------------------------------------------------------------------------
