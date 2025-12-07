#include "parser/check_variables.hpp"

#include "log/log_api.hpp"

namespace ParaCL
{

void ParserNameTable::new_scope()
{
    LOGINFO("paracl: parser: nametable: create new scope");
    scopes_.emplace_back();
}

void ParserNameTable::leave_scope()
{
    LOGINFO("paracl: parser: nametable: leave_scope current scope");

    if (scopes_.empty())
    {
        LOGERR("paracl: parser: nametable: warning: try to leave_scope from no-existence scope");
        return;
    }

    scopes_.pop_back();
}

bool ParserNameTable::is_declare(std::string_view variable) const
{
    LOGINFO("paracl: parser: nametable: check declaration of: \"{}\"", variable);

    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(std::string(variable));

        if (found == it->end())
            continue;

        LOGINFO("paracl: parser: nametable: redeclaration of \"{}\", return false", variable);
        return true;
    }

    LOGINFO("paracl: parser: nametable: variable NOT found: \"{}\"", variable);

    return false;
}

bool ParserNameTable::is_not_declare(std::string_view variable) const
{
    return (not is_declare(variable));
}

void ParserNameTable::declare_or_do_nothing_if_already_declared(std::string_view variable)
{
    LOGINFO("paracl: parser: nametable: try to declare variable: \"{}\"", variable);

    if (is_declare(variable))
    {
        LOGINFO("paracl: parser: nametable: \"{}\" already declared, return false", variable);
        return;
    }

    LOGINFO("paracl: parser: nametable: declare new variable \"{}\"", variable);
    (void)scopes_.back().insert(std::string(variable));
}

} /* namespace ParaCL */
