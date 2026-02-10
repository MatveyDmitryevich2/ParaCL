#ifndef NODE_HPP
#define NODE_HPP

#include "inode.hpp"
#include <memory>
#include <string>
#include <vector>

namespace language
{

class Number : public IExpression
{
private:
    int value_;

public:
    Number(int value);
    int evaluate() override;
    int get_value() const;
};

class Variable : public IExpression
{
private:
    std::string name_;

public:
    Variable(const std::string& name);
    int evaluate() override;
    const std::string& get_name() const;
};

class BinaryOp : public IExpression
{
public:
    enum class Op
    {
        ADD,
        SUB,
        MUL,
        DIV
    };

private:
    Op op_;
    std::unique_ptr<IExpression> left_;
    std::unique_ptr<IExpression> right_;

public:
    BinaryOp(Op op, std::unique_ptr<IExpression> left,
             std::unique_ptr<IExpression> right);
    int evaluate() override;

    Op get_op() const;
    IExpression* get_left() const;
    IExpression* get_right() const;
};

class Assignment : public IStatement
{
private:
    std::string var_name_;
    std::unique_ptr<IExpression> expr_;

public:
    Assignment(const std::string& var_name, std::unique_ptr<IExpression> expr);
    int evaluate() override;

    const std::string& get_var_name() const;
    IExpression* get_expr() const;
};

class PrintStmt : public IStatement
{
private:
    std::unique_ptr<IExpression> expr_;

public:
    PrintStmt(std::unique_ptr<IExpression> expr);
    int evaluate() override;

    IExpression* get_expr() const;
};

class BlockStmt : public IStatement
{
private:
    std::vector<std::unique_ptr<IStatement>> statements_;

public:
    BlockStmt();
    void add_statement(std::unique_ptr<IStatement> stmt);
    int evaluate() override;

    size_t get_statement_count() const;
    IStatement* get_statement(size_t index) const;
};

} // namespace language

#endif // NODE_HPP
