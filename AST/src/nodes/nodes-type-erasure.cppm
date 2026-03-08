module;

#include <memory>
#include <stdexcept>
#include <any>
#include <type_traits>
#include <utility>
#include <typeinfo>

export module node_type_erasure;

namespace last::node
{

export
namespace visit_specializations
{
template <typename NodeT, typename ReturnT, typename... Args>
ReturnT visit(NodeT const &, Args...)
{
    static_assert(false, "using not specialized visit function. "
                         "this static_assert help you to not see terribale linker errors :)");
}
} /* visit_specializations */

//--------------------------------------------------------------------------------------------------------------------------------------

export
class BasicNode
{
private:
    struct IBaseNode
    {
        virtual ~IBaseNode() = default;
        virtual std::unique_ptr<IBaseNode> clone_() const = 0;

        virtual std::type_info const & type_() const = 0;

        virtual std::any invoke_(const std::type_info& sig, std::any* args) const = 0;
        virtual bool supports_signature_(const std::type_info& sig) const = 0;
    };

    template<typename NodeT, typename... Signatures>
    requires (std::is_function_v<Signatures> && ...)
    struct NodeImpl final : public IBaseNode
    {
        NodeT data_;

        template<typename Signature>
        struct Invoker;

        template<typename ReturnT, typename... Args>
        struct Invoker<ReturnT(Args...)>
        {
            static std::any call_(const NodeT& data, std::any* args) try
            { return call_impl_(data, args, std::index_sequence_for<Args...>{}); }
            catch (const std::bad_any_cast& e)
            {
                throw std::runtime_error(
                    std::string("Type mismatch in arguments for signature ") + 
                    typeid(ReturnT(Args...)).name() + ": " + e.what()
                );
            }

            template<typename T>
            static T unwrap_arg_(const std::any& arg)
            {
                if constexpr (std::is_reference_v<T>)
                {
                    using WrappedType = std::reference_wrapper<std::remove_reference_t<T>>;
                    auto&& wrapper = std::any_cast<WrappedType&>(const_cast<std::any&>(arg));
                    return wrapper.get();
                }
                else
                    return std::any_cast<T>(arg);
            }

            template<size_t... Is>
            static std::any call_impl_(const NodeT& data, std::any* args, std::index_sequence<Is...>)
            {
                if constexpr (std::is_void_v<ReturnT>)
                {
                    visit_specializations::template visit<NodeT, ReturnT, Args...>(
                        data, 
                        unwrap_arg_<Args>(args[Is])...
                    );
                    return std::any{};
                }
                else
                {
                    auto&& result = visit_specializations::template visit<NodeT, ReturnT, Args...>(
                        data, 
                        unwrap_arg_<Args>(args[Is])...
                    );
                    return std::any(std::move(result));
                }
            }
        };

        std::type_info const & type_() const override
        { return typeid(NodeT); }

        template<size_t... Is>
        bool supports_signature_impl_(const std::type_info& sig, std::index_sequence<Is...>) const
        {
            bool supported = false;
            ((typeid(Signatures) == sig ? (supported = true) : false), ...);
            return supported;
        }

        template<size_t... Is>
        std::any invoke_impl_(const std::type_info& sig, std::any* args,
                             std::index_sequence<Is...>) const
        {
            std::any result;
            auto&& found = false;

            ((typeid(Signatures) == sig ? 
              (found = true, result = invoke_one_<Signatures>(args)) : 
              false), ...);

            if (found) return result;

            std::string supported;
            ((supported += typeid(Signatures).name(), supported += " "), ...);
            throw std::runtime_error(
                std::string("Signature ") + sig.name() + 
                " was not register for this node. supported:: " + supported
            );
        }

        template<typename Signature>
        std::any invoke_one_(std::any* args) const
        { return Invoker<Signature>::call_(data_, args); }

    public:
        explicit NodeImpl(NodeT&& node) : data_(std::forward<NodeT>(node)) {}

        std::unique_ptr<IBaseNode> clone_() const override
        {
            return std::unique_ptr<IBaseNode>(
                std::make_unique<NodeImpl>(*this).release()
            );
        }

        std::any invoke_(const std::type_info& sig, std::any* args) const override
        { return invoke_impl_(sig, args, std::index_sequence_for<Signatures...>{}); }

        bool supports_signature_(const std::type_info& sig) const override
        { return supports_signature_impl_(sig, std::index_sequence_for<Signatures...>{}); }
    };

    std::unique_ptr<IBaseNode> self_ = nullptr;

    explicit BasicNode(std::unique_ptr<IBaseNode> self) : self_(std::move(self)) {}

    template<typename T>
    static std::any pack_arg_(T&& arg)
    {
        if constexpr (std::is_lvalue_reference_v<T>)
            return std::ref(arg);
        else
            return std::any(std::forward<T>(arg));
    }

public:
    /*
    why not template ctor?
    in template ctor we need type deduction for all template types,
    but compiler cannot do this for functions signature, because it`s not using clearly
    */

    template <typename... Signatures>
    requires ((sizeof...(Signatures) > 0) && (std::is_function_v<Signatures> && ...))
    struct Actions
    {
        template<typename NodeT>
        static BasicNode create(NodeT&& node)
        {
            auto&& impl = std::make_unique<NodeImpl<NodeT, Signatures...>>(std::forward<NodeT>(node));
            return BasicNode(std::unique_ptr<IBaseNode>(impl.release()));
        }
    };

    /*
    fuction for visit basic node.
    if signature is no supported by the node - function will throw std::runtime_error
    */
    template<typename ReturnT, typename... Args>
    friend ReturnT visit(BasicNode const & node, Args... args)
    {
        if (not node.self_)
            throw std::runtime_error("visite node, which is not constructible (node.self_ is nullptr)");

        using SignatureT = ReturnT(Args...);

        if (not node.self_->supports_signature_(typeid(SignatureT))) 
        {
            throw std::runtime_error(
                std::string("This node dont support this function: ") + typeid(SignatureT).name()
            );
        }

        std::any arg_array[] = {pack_arg_(std::forward<Args>(args))...};
        auto&& result = node.self_->invoke_(
            typeid(SignatureT),
            arg_array
        );

        if constexpr (not std::is_void_v<ReturnT>)
        {
        try
        {
            return std::any_cast<ReturnT>(result);
        }
        catch (const std::bad_any_cast& e)
        {
            throw std::runtime_error(
                std::string("Return type mismatch: expected ") + 
                typeid(ReturnT).name() + 
                " but got " + 
                result.type().name()
            );
        }
        }
    }

    /*
    default ctor (need for nodes default ctor)
    warning: after this constructor visit will throw std::runtime_error
    self_ is nullptr
    */
    BasicNode() = default;

    /* copy ctor/assign */
    BasicNode(BasicNode const& other) 
        : self_(other.self_ ? other.self_->clone_() : nullptr)
    {}

    BasicNode& operator=(BasicNode const& other)
    {
        if (this == &other) return *this;
        self_ = other.self_ ? other.self_->clone_() : nullptr;
        return *this;
    }

    /* move ctor/assign */
    BasicNode(BasicNode&& other) noexcept = default;
    BasicNode& operator=(BasicNode&&) noexcept = default;


    /* check that node support some visit functions */
    template<typename... Signatures>
    requires (std::is_function_v<Signatures> && ...)
    bool support() const
    {
        return
            (self_) and
            (self_->supports_signature_(typeid(Signatures)) && ...);
    }

    /* check is the real node type T */
    template <typename T>
    bool is_a() const
    { return (typeid(T) == self_->type_()); }

    /* check that self is not nullptr */
    /* implicit */ operator bool() const noexcept
    { return static_cast<bool>(self_); }

    /* convert to a real data */
    template <typename T>
    requires (not std::is_same_v<std::remove_const_t<std::remove_reference_t<T>>, bool>)
    operator const T &() const
    {
        if (not is_a<T>())
            throw std::bad_cast{};

        return (static_cast<const NodeImpl<T>*>(self_.get()))->data_;
    }

    template <typename T>
    requires (not std::is_same_v<std::remove_const_t<std::remove_reference_t<T>>, bool>)
    operator T () const
    {
        if (not is_a<T>())
            throw std::bad_cast{};

        return (static_cast<const NodeImpl<T>*>(self_.get()))->data_;
    }
};

//--------------------------------------------------------------------------------------------------------------------------------------

/*
this function need to create BasicNode by your node.
in your code, specialize it for every node, you`re using.
use BasicNode::Actions<...>::create() for this.
*/

export
template <typename NodeT>
BasicNode create(NodeT)
{
    /*
    use this like:
    return BasicNode::Actions<__your_signatures__>::create(node)
    */
    static_assert(false, "using unspecialized create.");
};

//--------------------------------------------------------------------------------------------------------------------------------------
} /* namespace last::node */
//--------------------------------------------------------------------------------------------------------------------------------------
