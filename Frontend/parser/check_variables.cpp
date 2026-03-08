#include "check_variables.hpp"

#define LOGINFO(...)
#define LOGERR(...)

namespace ParaCL
{

void ParserNameTable::new_scope()
{
    LOGINFO("paracl: parser: nametable: create new scope (depth {})", 
                  scopes_.size() + 1);
    scopes_.emplace_back();
}

void ParserNameTable::leave_scope()
{
    LOGINFO("paracl: parser: nametable: leave scope (depth {})", 
                  scopes_.size());

    if (scopes_.empty())
    {
        LOGINFO("paracl: parser: nametable: try to leave non-existent scope");
        return;
    }
    scopes_.pop_back();
}

bool ParserNameTable::is_declare(std::string_view variable) const
{
    LOGINFO("paracl: parser: nametable: check declaration of: \"{}\"", variable);

    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        if (it->count(std::string(variable)))
        {
            LOGINFO("paracl: parser: nametable: \"{}\" FOUND → already declared, return true", 
                          variable);
            return true;
        }
    }

    LOGINFO("paracl: parser: nametable: \"{}\" NOT found", variable);
    return false;
}

bool ParserNameTable::is_not_declare(std::string_view variable) const
{
    return !is_declare(variable);
}

void ParserNameTable::declare_or_do_nothing_if_already_declared(std::string_view variable)
{
    LOGINFO("paracl: parser: nametable: try to declare: \"{}\"", variable);

    if (is_declare(variable))
    {
        LOGINFO("Parser nametable msg: Variable \"{}\" already exists", variable);
        return;
    }

    if (scopes_.empty())
    {
        LOGINFO("paracl: parser: nametable: ERROR: no active scope!");
        return;
    }

    scopes_.back().insert(std::string(variable));
    LOGINFO("Parser nametable msg: DECLARED \"{}\" in scope depth {}", 
                  variable, scopes_.size());
}

} // namespace ParaCL
