#include "check_variables.hpp"
#include <iostream>

namespace ParaCL
{

void ParserNameTable::new_scope()
{
    std::cerr << "paracl: parser: nametable: create new scope" << '\n';
    scopes_.emplace_back();
}

void ParserNameTable::leave_scope()
{
    std::cerr << "paracl: parser: nametable: leave_scope current scope" << '\n';

    if (scopes_.empty())
    {
        std::cerr << "paracl: parser: nametable: warning: try to leave_scope from no-existence scope" << '\n';
        return;
    }

    scopes_.pop_back();
}

bool ParserNameTable::is_declare(std::string_view variable) const
{
    std::cerr << "paracl: parser: nametable: check declaration of: \"{}\"" << variable << '\n';

    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        auto found = it->find(std::string(variable));

        if (found == it->end())
            continue;

        std::cerr << "paracl: parser: nametable: redeclaration of \"{}\", return false" << variable << '\n';
        return true;
    }

    std::cerr << "paracl: parser: nametable: variable NOT found: \"{}\"" << variable << '\n';

    return false;
}

bool ParserNameTable::is_not_declare(std::string_view variable) const
{
    return (not is_declare(variable));
}

void ParserNameTable::declare_or_do_nothing_if_already_declared(std::string_view variable)
{
    std::cerr << "paracl: parser: nametable: try to declare variable: \"{}\"" << variable << '\n';

    if (is_declare(variable))
    {
        std::cerr << "paracl: parser: nametable: \"{}\" already declared, return false" << variable << '\n';
        return;
    }

    std::cerr << "paracl: parser: nametable: declare new variable \"{}\"" << variable << '\n';
    (void)scopes_.back().insert(std::string(variable));
}

} /* namespace ParaCL */
