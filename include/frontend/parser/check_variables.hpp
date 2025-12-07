#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace ParaCL
{

struct ParserNameTable
{
  private:
    std::vector<std::unordered_set<std::string>> scopes_;

  public:
    ParserNameTable() = default;
    void new_scope();
    void leave_scope();
    bool is_not_declare(std::string_view variable) const;
    bool is_declare(std::string_view variable) const;

    void declare_or_do_nothing_if_already_declared(std::string_view variable);
};

} /* namespace ParaCL*/
