module;

//---------------------------------------------------------------------------------------------------------------

#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#define LOGINFO(...)
#define LOGERR(...)

//---------------------------------------------------------------------------------------------------------------

export module nametable;

//---------------------------------------------------------------------------------------------------------------

namespace interpreter::nametable
{

//---------------------------------------------------------------------------------------------------------------

export
class Nametable final
{
  private:
    std::vector<std::unordered_map<std::string_view, int>> scopes_;
  private:
    int* lookup            (std::string_view name);
    void declare           (std::string_view name, int value);
  public:
    void new_scope         ();
    void leave_scope       ();
    void set_value         (std::string_view name, int value);
    int  get_variable_value(std::string_view name) const;
};

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

    if (scopes_.empty()) return;
    scopes_.pop_back();
}

//---------------------------------------------------------------------------------------------------------------

int Nametable::get_variable_value(std::string_view name) const
{
    LOGINFO("paracl: interpreter: nametable: searching variable: \"{}\"", name);

    for (auto&& scope : scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
        LOGINFO("paracl: interpreter: nametable: variable found: \"{}\" = {}", name, found->second);
        return found->second;
    }

    LOGINFO("paracl: interpreter: nametable: variable NOT found: \"{}\"", name);
    throw std::runtime_error(std::string("requests value of not exists variable: ") + std::string(name));
}

//---------------------------------------------------------------------------------------------------------------

void Nametable::set_value(std::string_view name, int value)
{
    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value, name);

    if (scopes_.empty())
        throw std::runtime_error("cannot set_value variable: no active scopes");

    auto&& name_ptr = lookup(name);

    if (not name_ptr) return declare(name, value);

    LOGINFO("paracl: interpreter: nametable: set {} to \"{}\"", value, name);
    *name_ptr = value;
}

// private
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

int *Nametable::lookup(std::string_view name)
{
    for (auto &&scope: scopes_ | std::views::reverse)
    {
        auto&& found = scope.find(name);
        if (found == scope.end()) continue;
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
} /* namespace ParaCL::backend::interpreter::nametable */
//---------------------------------------------------------------------------------------------------------------
