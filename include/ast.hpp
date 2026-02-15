#ifndef AST_HPP
#define AST_HPP

#include "node.hpp"
#include <memory>
#include <string>

namespace language
{

class AST
{
private:
    std::unique_ptr<BlockStmt> root_;

public:
    AST() : root_(std::make_unique<BlockStmt>())
    {
    }

    void add_assignment(std::string var_name, IExpression* expr)
    {
        root_->add_statement(std::make_unique<Assignment>(
            std::move(var_name), std::unique_ptr<IExpression>(expr)));
    }

    void add_if(IExpression* cond, IStatement* then_branch,
                IStatement* else_branch = nullptr)
    {
        root_->add_statement(std::make_unique<IfStmt>(
            std::unique_ptr<IExpression>(cond),
            std::unique_ptr<IStatement>(then_branch),
            else_branch ? std::unique_ptr<IStatement>(else_branch) : nullptr));
    }

    void add_while(IExpression* cond, IStatement* body)
    {
        root_->add_statement(
            std::make_unique<WhileStmt>(std::unique_ptr<IExpression>(cond),
                                        std::unique_ptr<IStatement>(body)));
    }

    void add_print(IExpression* expr)
    {
        root_->add_statement(
            std::make_unique<PrintStmt>(std::unique_ptr<IExpression>(expr)));
    }

    IExpression* create_number(int value)
    {
        return new Number(value);
    }

    IExpression* create_variable(std::string* name)
    {
        IExpression* expr = new Variable(*name);
        return expr;
    }
    IExpression* create_binary_op(BinaryOp::Op op, IExpression* left,
                                  IExpression* right)
    {
        return new BinaryOp(op, std::unique_ptr<IExpression>(left),
                            std::unique_ptr<IExpression>(right));
    }

    IExpression* create_unary(UnaryOp::Op op, IExpression* expr)
    {
        return new UnaryOp(op, std::unique_ptr<IExpression>(expr));
    }
    IExpression* create_scanf()
    {
        return new ScanfExpr();
    }

    int run()
    {
        return root_->evaluate();
    }
    // ===== ДЛЯ ОТЛАДКИ ДЕРЕВА =====
    const BlockStmt* get_root() const
    {
        return root_.get();
    }

    void set_root(BlockStmt* new_root)
    {
        root_.reset(new_root);
    }
};

} // namespace language

#endif // AST_HPP
