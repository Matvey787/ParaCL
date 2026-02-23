module;

#include <utility>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

export module ast_nodes;

import node_type_erasure;

namespace ParaCL
{
namespace ast
{
namespace node
{

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Scope final : private std::vector<BasicNode>
{
public:
    using std::vector<BasicNode>::emplace_back;
    using std::vector<BasicNode>::begin;
    using std::vector<BasicNode>::end;
    using std::vector<BasicNode>::size;

public:
    Scope() = default;
    Scope(size_t size) : std::vector<BasicNode>(size)
    {}
    Scope(BasicNode&& node) : std::vector<BasicNode>(1)
    { this->emplace_back(node); } 

    Scope(std::vector<BasicNode>&& nodes) : std::vector<BasicNode>(std::move(nodes))
    {}
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Variable final
{
private:
    std::string name_;
public:
    Variable(std::string&& name) : name_(std::move(name))
    {}
public:
    std::string_view name() const & noexcept
    { return name_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Print final : private std::vector<BasicNode>
{
public:
    using std::vector<BasicNode>::emplace_back;
    using std::vector<BasicNode>::begin;
    using std::vector<BasicNode>::end;
    using std::vector<BasicNode>::size;
public:
    Print() = default;
    Print(std::vector<BasicNode>&& args) : std::vector<BasicNode>(std::move(args))
    {}
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Scan final
{
public:
    Scan() = default;
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class UnaryOperator final
{
public:
    enum UnaryOperatorT
    { MINUS, PLUS, NOT };
private:
    BasicNode arg_;
    UnaryOperatorT type_;
public:
    UnaryOperator(UnaryOperatorT type, BasicNode&& arg) :
    type_(type), arg_(std::move(arg))
    {}
public:
    UnaryOperatorT type() const noexcept
    { return type_; }

    BasicNode const &arg() const & noexcept
    { return arg_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class BinaryOperator final
{
public:
    enum BinaryOperatorT
    {
        AND,
        OR,
        NOT,
        ADD,
        SUB,
        MUL,
        DIV,
        REM,
        ISAB,
        ISABE,
        ISLS,
        ISLSE,
        ISEQ,
        ISNE,
        ASGN,    /* =  */
        ADDASGN, /* += */
        SUBASGN, /* -= */
        MULASGN, /* *= */
        DIVASGN, /* /= */
        REMASGN, /* %= */
    };

private:
    BasicNode larg_;
    BasicNode rarg_;
    BinaryOperatorT type_;
public:
    BinaryOperator(BinaryOperatorT type, BasicNode&& larg, BasicNode&& rarg) :
    larg_(std::move(larg)), rarg_(std::move(rarg)), type_(type)
    {}
public:
    BinaryOperatorT type() const noexcept
    { return type_; }
    BasicNode const &larg() const & noexcept
    { return larg_; }
    BasicNode const &rarg() const & noexcept
    { return rarg_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class NumberLiteral final
{
private:
    int value_;
public:
    explicit NumberLiteral(int value) : value_(value)
    {}
public:
    int value() const noexcept
    { return value_; };
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class StringLiteral final
{
private:
    std::string value_;
public:
    StringLiteral(std::string&& value) :
    value_(std::move(value))
    {}
    StringLiteral(std::string_view value) :
    value_(value)
    {}
public:
    std::string_view value() const & noexcept
    { return value_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

/* impl basic class for while, if, else-if classes */
class ConditionWithBody /* not final */
{
private:
    BasicNode condition_;
    BasicNode body_;
protected:
    ConditionWithBody() = default;
    ConditionWithBody(BasicNode&& condition, BasicNode&& body) :
    condition_(std::move(condition)), body_(std::move(body))
    {}
public:
    BasicNode const &condition() const & noexcept
    { return condition_; }
    BasicNode const &body() const & noexcept
    { return body_; };
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class While final : public ConditionWithBody
{
public:
    using ConditionWithBody::ConditionWithBody;
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class If final : public ConditionWithBody
{
public:
    using ConditionWithBody::ConditionWithBody;
};

//--------------------------------------------------------------------------------------------------------------------------------------

export
class Condition;

export
class Else final
{
private:
    BasicNode body_;
public:
    Else(BasicNode&& body) : body_(std::move(body))
    {}
public:
    BasicNode const &boby() const & noexcept
    { return body_; }

    friend class Condition;

private:
    Else() = default;
};

//--------------------------------------------------------------------------------------------------------------------------------------

class Condition final
{
private:
    std::vector<BasicNode> ifs_;
    BasicNode else_;
public:
    Condition() = default;
    Condition(std::vector<BasicNode>&& ifs, BasicNode&& else_а_как_вот_это_назвать) :
       ifs_(std::move(ifs)), else_(std::move(else_а_как_вот_это_назвать))
    {}
public:
    void add_condition(BasicNode&& condition)
    { ifs_.emplace_back(std::move(condition)); }

    void set_else(BasicNode&& else_а_как_вот_это_назвать)
    { else_ = std::move(else_а_как_вот_это_назвать); }

public:

    std::vector<BasicNode> const &get_ifs() const & noexcept
    { return ifs_; }

    BasicNode const &get_else() const & noexcept
    { return else_; }
};

//--------------------------------------------------------------------------------------------------------------------------------------

} /* namespace node */
} /* namespace node */
} /* namespace ParaCL */

//--------------------------------------------------------------------------------------------------------------------------------------
