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

    template <typename T, typename... Args> T* make_node(Args&&... args)
    {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = ptr.get();
        owned_nodes_.push_back(std::move(ptr));
        return raw;
    }

public:
    AST() : root_(make_node<BlockStmt>())
    {
    }
    ~AST() = default;

    IExpression* create_number(int value)
    {
        return make_node<Number>(value);
    }

    IExpression* create_variable(const std::string& name)
    {
        return make_node<Variable>(name);
    }

    IExpression* create_binary_op(BinaryOp::Op op, IExpression* left,
                                  IExpression* right)
    {
        return make_node<BinaryOp>(op, left, right);
    }

    IExpression* create_unary(UnaryOp::Op op, IExpression* expr)
    {
        return make_node<UnaryOp>(op, expr);
    }

    IExpression* create_scanf()
    {
        return make_node<ScanfExpr>();
    }

    IStatement* create_assignment(const std::string& var_name,
                                  IExpression* expr)
    {
        return make_node<Assignment>(var_name, expr);
    }

    IStatement* create_declaration(const std::string& var_name,
                                   IExpression* expr = nullptr)
    {
        return make_node<Declaration>(var_name, expr);
    }

    IStatement* create_print(IExpression* expr)
    {
        return make_node<PrintStmt>(expr);
    }

    IStatement* create_if(IExpression* cond, IStatement* then_branch,
                          IStatement* else_branch = nullptr)
    {
        return make_node<IfStmt>(cond, then_branch, else_branch);
    }

    IStatement* create_while(IExpression* cond, IStatement* body)
    {
        return make_node<WhileStmt>(cond, body);
    }

    BlockStmt* create_block()
    {
        return make_node<BlockStmt>();
    }

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

    void add_statement_to_block(BlockStmt* block, IStatement* stmt)
    {
        block->add_statement(stmt);
    }
};

} // namespace language

#endif // AST_HPP
