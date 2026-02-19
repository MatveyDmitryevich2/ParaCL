#ifndef NODE_HPP
#define NODE_HPP

#include "fwd.hpp"
#include "inode.hpp"
#include <memory>
#include <string>
#include <vector>
#include <cstddef>

namespace language
{

class Number : public IExpression
{
private:
    int value_;

public:
    Number(int value) : value_(value)
    {
    }
    void evaluate(Interpreter& interp) override;
    int get_value() const
    {
        return value_;
    }
    ~Number() = default;
};

class Variable : public IExpression
{
private:
    std::string name_;

public:
    Variable(const std::string& name) : name_(name)
    {
    }
    void evaluate(Interpreter& interp) override;
    const std::string& get_name() const
    {
        return name_;
    }
    ~Variable() = default;
};

class BinaryOp : public IExpression
{
public:
    enum class Op { ADD, SUB, MUL, DIV, OR, AND, EQ, NE, L, G, LE, GE };

private:
    Op op_;
    IExpression* left_;
    IExpression* right_;

public:
    BinaryOp(Op op, IExpression* left, IExpression* right)
        : op_(op), left_(left), right_(right)
    {
    }
    void evaluate(Interpreter& interp) override;
    Op get_op() const
    {
        return op_;
    }
    IExpression* get_left() const
    {
        return left_;
    }
    IExpression* get_right() const
    {
        return right_;
    }

    ~BinaryOp() = default;
};

class UnaryOp : public IExpression
{
public:
    enum class Op
    {
        MINUS,
        NOT
    };

private:
    Op op_;
    IExpression* expr_;

public:
    UnaryOp(Op op, IExpression* expr) : op_(op), expr_(expr)
    {
    }
    void evaluate(Interpreter& interp) override;
    Op get_op() const
    {
        return op_;
    }
    IExpression* get_expr() const
    {
        return expr_;
    }
    ~UnaryOp() = default;
};

class Assignment : public IStatement
{
private:
    std::string var_name_;
    IExpression* expr_;

public:
    Assignment(const std::string& var_name, IExpression* expr)
        : var_name_(var_name), expr_(expr)
    {
    }
    void evaluate(Interpreter& interp) override;
    const std::string& get_var_name() const
    {
        return var_name_;
    }
    IExpression* get_expr() const
    {
        return expr_;
    }
    ~Assignment() = default;
};

class PrintStmt : public IStatement
{
private:
    IExpression* expr_;

public:
    PrintStmt(IExpression* expr) : expr_(expr)
    {
    }
    void evaluate(Interpreter& interp) override;
    IExpression* get_expr() const
    {
        return expr_;
    }
    ~PrintStmt() = default;
};

class ScanfExpr : public IExpression
{
public:
    ScanfExpr() = default;
    void evaluate(Interpreter& interp) override;
    ~ScanfExpr() = default;
};

class IfStmt : public IStatement
{
private:
    IExpression* condition_;
    IStatement* body_if_;
    IStatement* body_else_;

public:
    IfStmt(IExpression* condition, IStatement* body_if,
           IStatement* body_else = nullptr)
        : condition_(condition), body_if_(body_if), body_else_(body_else)
    {
    }
    void evaluate(Interpreter& interp) override;
    IExpression* get_condition() const
    {
        return condition_;
    }
    IStatement* get_body_if() const
    {
        return body_if_;
    }
    IStatement* get_body_else() const
    {
        return body_else_;
    }
    ~IfStmt() = default;
};

class WhileStmt : public IStatement
{
private:
    IExpression* condition_;
    IStatement* body_;

public:
    WhileStmt(IExpression* condition, IStatement* body)
        : condition_(condition), body_(body)
    {
    }
    void evaluate(Interpreter& interp) override;
    IExpression* get_condition() const
    {
        return condition_;
    }
    IStatement* get_body() const
    {
        return body_;
    }
    ~WhileStmt() = default;
};

class BlockStmt : public IStatement
{
private:
    std::vector<IStatement*> statements_;

public:
    BlockStmt() = default;
    void add_statement(IStatement* stmt)
    {
        statements_.push_back(stmt);
    }
    void evaluate(Interpreter& interp) override;
    size_t get_statement_count() const
    {
        return statements_.size();
    }
    IStatement* get_statement(size_t i) const
    {
        return statements_[i];
    }
    ~BlockStmt() = default;
};

} // namespace language

#endif // NODE_HPP