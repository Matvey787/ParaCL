module;

#include <memory>
#include <filesystem>
#include <fstream>
#include <stdexcept>

export module node_type_erasure;



namespace ParaCL::ast::node
{

export
namespace visit_overload_set
{
template <typename NodeT, typename ReturnT, typename... Args>
ReturnT visit(NodeT const &node, Args... args)
{
    static_assert(false, "using not specialized visit function. "
                         "this static_assert help you to not think about terribale linker errors :)");
}

template <typename NodeT>
void write(NodeT const &node, std::ofstream& ofs, size_t enclosure)
{
    static_assert(false, "using not specialized write function. "
                         "this static_assert help you to not think about terribale linker errors :)");
}
} /* visit_overload_set */

//--------------------------------------------------------------------------------------------------------------------------------------

export
class BasicNode
{
private:
    struct IBaseNode
    {
        virtual ~IBaseNode() = default;
        virtual std::unique_ptr<IBaseNode> clone_() const = 0;
        virtual void write_(std::ofstream& ofs, size_t enclosure) const = 0;
    };

    template <typename ReturnT, typename... Args>
    struct IVisitable : public IBaseNode
    {
        virtual ReturnT visit_(Args... args) const = 0;
    };

    template <typename NodeT, typename ReturnT, typename... Args>
    struct Node final : public IVisitable<ReturnT, Args...>
    {
        NodeT data_;

        Node(NodeT node) : data_(std::move(node))
        {}

        std::unique_ptr<IBaseNode> clone_() const override
        { return std::make_unique<Node<NodeT, ReturnT, Args...>>(*this); }

        ReturnT visit_(Args... args) const override
        { return visit_overload_set::template visit<NodeT, ReturnT, Args...>(data_, args...); }

        void write_(std::ofstream& ofs, size_t enclosure) const override
        { return visit_overload_set::template write<NodeT>(data_, ofs, enclosure); }
    };

    std::unique_ptr<IBaseNode> self_ = nullptr;

private:
    BasicNode(std::unique_ptr<IBaseNode> node) : self_(std::move(node))
    {}

public:
    template <typename ReturnT, typename... Args>
    friend ReturnT visit(BasicNode const &node, Args... args)
    {
        using expected_node_type = IVisitable<ReturnT, Args...>;
        auto&& self_ptr = dynamic_cast<const expected_node_type*>(node.self_.get());
        if (self_ptr) return self_ptr->visit_(args...);
        throw std::runtime_error("Using fuction, not spezified for this node.");
    }

    friend void write(BasicNode const &node, std::ofstream& ofs, size_t enclosure)
    { return node.self_->write_(ofs, enclosure); }

    BasicNode() = default;

    /* explicit - because for all node-type we need separate constructor */
    template <typename NodeT, typename ReturnT, typename... Args>
    explicit
    BasicNode(NodeT node) :
        self_(std::make_unique<Node<NodeT, ReturnT, Args...>>(std::move(node)))
    {}

    template <typename NodeT, typename ReturnT, typename... Args>
    static
    BasicNode create(NodeT node)
    { return BasicNode{std::make_unique<Node<NodeT, ReturnT, Args...>>(std::move(node))}; }

    BasicNode(BasicNode const &other) :
        self_(other.self_ ? other.self_->clone_() : nullptr)
    {}

    BasicNode &operator=(BasicNode const &other)
    {
        other.self_->clone_().swap(self_);
        return *this;
    }

    BasicNode(BasicNode &&) = default;
    BasicNode &operator=(BasicNode &&) = default;
};

//--------------------------------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::ast::node */
//--------------------------------------------------------------------------------------------------------------------------------------
