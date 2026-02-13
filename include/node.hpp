#ifndef NODE_HPP
#define NODE_HPP

#include "inode.hpp"
#include <memory>
#include <string>
#include <vector>

// FIXME косяк с тем что тут не всегда виден interpreter

namespace language
{

class Number : public IExpression
{
private:
    int value_;

public:
    //Number(int value);
    void evaluate(Interpreter& interp) override;
    //int get_value() const;
};

class Variable : public IExpression
{
private:
    std::string name_;

public:
    //Variable(const std::string& name);
    void evaluate(Interpreter& interp) override;
    //const std::string& get_name() const;
};

class Declaration : public IStatement
{
private:
    std::string name_;
    std::unique_ptr<IExpression> expr_;

public:
    void evaluate(Interpreter& interp) override;
};

class BinaryOp : public IExpression
{
public:
    enum class Op { ADD, SUB, MUL, DIV, OR, AND, EQ, NE, L, G, LE, GE };

private:
    Op op_;
    std::unique_ptr<IExpression> left_expr_;
    std::unique_ptr<IExpression> right_expr_;

public:
    void evaluate(Interpreter& interp) override;
};

class Assignment : public IStatement
{
private:
    std::string name_;
    std::unique_ptr<IExpression> expr_;

public:
    void evaluate(Interpreter& interp) override;
};

class PrintStmt : public IStatement
{
private:
    std::unique_ptr<IExpression> expr_;

public:
    void evaluate(Interpreter& interp) override;
};

class ScanfExpr : public IExpression
{
public:
    void evaluate(Interpreter& interp) override;
};

class IfStmt : public IStatement
{
private:
    std::unique_ptr<IExpression> condition_;
    std::unique_ptr<IStatement> body_if_;
    std::unique_ptr<IStatement> body_else_; // nullptr если нет else

public:
    void evaluate(Interpreter& interp) override;
};

class WhileStmt : public IStatement
{
private:
    std::unique_ptr<IExpression> condition_;
    std::unique_ptr<IStatement> body_;

public:
    void evaluate(Interpreter& interp) override;
};

class BlockStmt : public IStatement
{
private:
    std::vector<std::unique_ptr<IStatement>> statements_;

public:
    void evaluate(Interpreter& interp) override;
};
} // namespace language

#endif // NODE_HPP