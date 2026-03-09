#ifndef AST_HPP
#define AST_HPP

#include "node.hpp"
#include <memory>
#include <string>
#include <vector>

namespace language
{

class AST
{
private:
    std::vector<std::unique_ptr<INode>> owned_nodes_;
    std::vector<std::unique_ptr<std::string>> owned_strings_;
    BlockStmt* root_;

public:
    template <typename T, typename... Args> T* make_node(Args&&... args)
    {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = ptr.get();
        owned_nodes_.push_back(std::move(ptr));
        return raw;
    }

    AST() : root_(make_node<BlockStmt>())
    {
    }

    ~AST() = default;

    std::string* make_string(const std::string& s)
    {
        auto ptr = std::make_unique<std::string>(s);
        std::string* raw = ptr.get();
        owned_strings_.push_back(std::move(ptr));
        return raw;
    }

    void set_root(BlockStmt* new_root)
    {
        root_ = new_root;
    }

    BlockStmt* get_root() const
    {
        return root_;
    }
};

} // namespace language

#endif // AST_HPP
