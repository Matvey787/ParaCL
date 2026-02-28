module;

// import ast_concepts;

#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <iostream>
#include <assert.h>

export module ast;

export namespace ParaCL::front::AST
{

namespace visit_overload_set
{

template <typename NodeT, typename ReturnT, typename... Args>
ReturnT visit(NodeT const &node, Args... args)
{
    assert(false && "using not specialized visit function. "
                 "this static_assert help you to not think about terribale linker errors :)");
}

} // namespace visit_overload_set

// Type-Erased class + Visitor pattern
// Required for storing classes of absolutely different types
// Read about it here https://habr.com/ru/articles/706450/?ysclid=mlzkvs6r2x343771650
class AnonNode
{
    class INode
    {
    public:

        virtual ~INode() = default;
        virtual std::unique_ptr<INode> clone() const = 0;
        virtual const std::type_info& type() const = 0;
    };

    template<typename RetT, typename... Args>
    class IVisitable
    {
    public:

        virtual ~IVisitable() = default;
        virtual RetT visit(Args... args) const = 0; // Why virtual?
    };


    template<typename NodeT, typename RetT, typename... Args>
    class Node final : public INode, public IVisitable<RetT, Args...>
    {
    public:
        
        NodeT data_;

        Node(NodeT nodeObj) : data_(std::move(nodeObj)) {}
        
        std::unique_ptr<INode> clone() const override
        {
            return std::unique_ptr<INode>(
                new Node<NodeT, RetT, Args...>(data_)
            );
        }

        const std::type_info& type() const override
        {
            return typeid(NodeT);
        }

        RetT visit(Args... args) const override
        {
            return visit_overload_set::visit<NodeT, RetT, Args...>(data_, std::forward<Args>(args)...);
        }
    };

    std::unique_ptr<INode> pimpl_;

public:
    AnonNode() = default;
    
    template<typename NodeT, typename RetT = void, typename... Args>
    explicit AnonNode(NodeT node)
        : pimpl_(new Node<NodeT, RetT, Args...>(std::move(node))) {}

    template<typename NodeT, typename RetT = void, typename... Args>
    static AnonNode create(NodeT node)
    {
        return AnonNode(new Node<NodeT, RetT, Args...>(std::move(node)));
    }
    
    AnonNode(const AnonNode& other)
        : pimpl_(other.pimpl_ ? other.pimpl_->clone() : nullptr) {}
    
    AnonNode(AnonNode&&) = default;
    
    AnonNode& operator=(const AnonNode& other)
    {
        if (this != &other && other.pimpl_)
            pimpl_ = other.pimpl_->clone();
        return *this;
    }
    
    AnonNode& operator=(AnonNode&&) = default;
    
    template<typename RetT = void, typename... Args>
    RetT visit(Args... args) const
    {
        if (!pimpl_) throw std::runtime_error("Empty node");
        
        using Visitable = IVisitable<RetT, Args...>;
        auto* visitable = dynamic_cast<const Visitable*>(pimpl_.get());
        
        if (!visitable)
            throw std::runtime_error("This node doesn't support visit with these arguments");
        
        return visitable->visit(std::forward<Args>(args)...);
    }
    
    explicit operator bool() const noexcept { return static_cast<bool>(pimpl_); }
};


class NumberLiteral final
{
private:
    int value_;
public:
    explicit NumberLiteral(int value) : value_(value) {}
    
    int value() const noexcept { return value_; }
};


template<typename T>
concept ConvToStrView = std::convertible_to<T, std::string_view>;

class StringLiteral final
{
private:
    std::string value_;

public:

    template <ConvToStrView T>
    explicit StringLiteral(T&& value) : value_(value) {}
    
    std::string_view value() const & noexcept { return value_; }
};


class Variable final
{
private:
    std::string name_;
public:
    template <ConvToStrView T>
    Variable(T&& name) : name_(std::move(name)) {}
    
    std::string_view name() const & noexcept { return name_; }
};


class Scan final
{
public:
    Scan() = default;
};


class Print final
{
private:
    std::vector<AnonNode> args_;
public:
    Print() = default;
    Print(std::vector<AnonNode>&& args) : args_(std::move(args)) {}
    
    void addArg(AnonNode&& arg) { args_.push_back(std::move(arg)); }
    
    const std::vector<AnonNode>& args() const { return args_; }
};


class UnaryOperator final
{
public:
    enum class UnaryOperatorT
    {
        MINUS,
        PLUS,
        NOT
    };

private:
    UnaryOperatorT type_;
    AnonNode arg_;

public:
    UnaryOperator(UnaryOperatorT type, AnonNode&& arg)
        : type_(type), arg_(std::move(arg)) {}
    
    UnaryOperatorT type() const noexcept { return type_; }
    const AnonNode& arg() const & noexcept { return arg_; }
};


class BinaryOperator final
{
public:
    enum class BinaryOperatorT
    {
        AND,        // &&
        OR,         // ||
        ADD,        // +
        SUB,        // -
        MUL,        // *
        DIV,        // /
        REM,        // %
        ISAB,       // >
        ISABE,      // >=
        ISLS,       // <
        ISLSE,      // <=
        ISEQ,       // ==
        ISNE,       // !=
        ASGN,       // =
        ADDASGN,    // +=
        SUBASGN,    // -=
        MULASGN,    // *=
        DIVASGN,    // /=
        REMASGN,    // %=
    };

private:
    BinaryOperatorT type_;
    AnonNode larg_;
    AnonNode rarg_;

public:
    BinaryOperator(BinaryOperatorT type, AnonNode&& larg, AnonNode&& rarg)
        : type_(type), larg_(std::move(larg)), rarg_(std::move(rarg)) {}
    
    BinaryOperatorT type() const noexcept { return type_; }
    const AnonNode& larg() const & noexcept { return larg_; }
    const AnonNode& rarg() const & noexcept { return rarg_; }
};


class ConditionWithBody
{
private:
    AnonNode condition_;
    AnonNode body_;

public:
    ConditionWithBody() = default;
    ConditionWithBody(AnonNode&& condition, AnonNode&& body)
        : condition_(std::move(condition)), body_(std::move(body)) {}
    
public:
    const AnonNode& condition() const & noexcept { return condition_; }
    const AnonNode& body() const & noexcept { return body_; }
};


class While final : public ConditionWithBody
{
public:
    using ConditionWithBody::ConditionWithBody;
};


class If final : public ConditionWithBody
{
public:
    using ConditionWithBody::ConditionWithBody;
};


class Else final
{
private:
    AnonNode body_;

public:
    Else() = default;
    explicit Else(AnonNode&& body) : body_(std::move(body)) {}
    
    const AnonNode& body() const & noexcept { return body_; }
};


class Condition final
{
private:
    std::vector<AnonNode> ifs_;
    AnonNode else_;

public:
    Condition() = default;
    Condition(std::vector<AnonNode>&& ifs, AnonNode&& elseNode)
        : ifs_(std::move(ifs)), else_(std::move(elseNode)) {}
    
    void addIf(AnonNode&& ifNode)
    {
        ifs_.push_back(std::move(ifNode));
    }
    
    void setElse(AnonNode&& elseNode)
    {
        else_ = std::move(elseNode);
    }
    
    const std::vector<AnonNode>& getIfs() const & noexcept { return ifs_; }
    const AnonNode& getElse() const & noexcept { return else_; }
    
    bool hasElse() const { return static_cast<bool>(else_); }
};


class Scope final
{
private:
    std::vector<AnonNode> statements_;

public:
    Scope() = default;
    explicit Scope(size_t size) : statements_(size) {}
    explicit Scope(AnonNode&& node)
    {
        statements_.push_back(std::move(node));
    }
    explicit Scope(std::vector<AnonNode>&& nodes) : statements_(std::move(nodes)) {}
    
    void addStatement(AnonNode&& stmt)
    {
        statements_.push_back(std::move(stmt));
    }
    
    const std::vector<AnonNode>& statements() const { return statements_; }
    
    auto begin() const { return statements_.begin(); }
    auto end() const { return statements_.end(); }
    size_t size() const { return statements_.size(); }
};


class ProgramAST final
{
private:
    std::vector<AnonNode> statements_;

public:
    ProgramAST() = default;
    explicit ProgramAST(std::vector<AnonNode>&& stmts) : statements_(std::move(stmts)) {}
    
    void addStatement(AnonNode&& stmt)
    {
        statements_.push_back(std::move(stmt));
    }
    
    const std::vector<AnonNode>& getStatements() const { return statements_; }
};

template<typename T, typename... Args>
inline AnonNode make_node(Args&&... args)
{
    return AnonNode::create<T>(T(std::forward<Args>(args)...));
}

} // namespace ParaCL::front::AST