module;

#include <memory>

export module ast;

export import node_type_erasure;
export import ast_nodes;

namespace last
{


export
class AST
{
private:
    node::BasicNode root_;

public:
    AST(node::BasicNode const & root) : /* too long, because all tree will copy */
        root_(root)
    {}
    
    AST() = default;

    AST(node::BasicNode&& root) :
        root_(std::move(root))
    {}

    

public:
    node::BasicNode const &root() const noexcept
    { return root_; }

};

} /* namespace last */
