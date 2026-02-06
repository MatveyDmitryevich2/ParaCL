#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>      // Std::cout
#include <memory>        // For std::unique_ptr
#include <string>        // For std::string
#include <unordered_map> // Symbol table
#include <vector>        // For std::vector

namespace language
{
class Node
{
public:
    virtual ~Node() = default;
    virtual int evaluate() = 0;
};

class Expression : public Node
{
};

class Statement : public Node
{
};

class Number : public Expression
{
private:
    int value_;

public:
    Number(int value) : value_(value)
    {
    }
    int evaluate() override
    {
        return value_;
    }
};

class Variable : public Expression
{
private:
    std::string name_;
    static std::unordered_map<std::string, int> memory_;

public:
    Variable(const std::string& name) : name_(name)
    {

        if (memory_.find(name) == memory_.end())
        {
            memory_[name] = 0;
        }
    }

    int evaluate() override
    {
        return memory_[name_];
    }

    static void set_value(const std::string& name, int value)
    {
        memory_[name] = value;
    }
};

class BinaryOp : public Expression
{
private:
    enum class Op
    {
        ADD,
        SUB,
        MUL,
        DIV,
        EQ,
        NE,
        LT,
        GT,
        LE,
        GE,
        AND,
        OR
    } op_;

    std::unique_ptr<Expression> left_;
    std::unique_ptr<Expression> right_;

public:
    BinaryOp(Op op, std::unique_ptr<Expression> left,
             std::unique_ptr<Expression> right)
        : op_(op), left_(std::move(left)), right_(std::move(right))
    {
    }

    int evaluate() override
    {
        int left_val = left_->evaluate();
        int right_val = right_->evaluate();

        switch (op_)
        {
        // Арифметика
        case Op::ADD:
            return left_val + right_val;
        case Op::SUB:
            return left_val - right_val;
        case Op::MUL:
            return left_val * right_val;
        case Op::DIV:
            if (right_val == 0)
            {
                throw std::runtime_error("Division by zero!");
            }
            return left_val / right_val;

        case Op::EQ:
            return left_val == right_val ? 1 : 0;
        case Op::NE:
            return left_val != right_val ? 1 : 0;
        case Op::LT:
            return left_val < right_val ? 1 : 0;
        case Op::GT:
            return left_val > right_val ? 1 : 0;
        case Op::LE:
            return left_val <= right_val ? 1 : 0;
        case Op::GE:
            return left_val >= right_val ? 1 : 0;

        case Op::AND:
            return (left_val != 0 && right_val != 0) ? 1 : 0;
        case Op::OR:
            return (left_val != 0 || right_val != 0) ? 1 : 0;

        default:
            return 0;
        }
    }
};

class Assignment : public Statement
{
private:
    std::string var_name_;
    std::unique_ptr<Expression> expr_;

public:
    // Конструктор
    Assignment(const std::string& var_name, std::unique_ptr<Expression> expr)
        : var_name_(var_name), expr_(std::move(expr))
    {
    }

    int evaluate() override
    {

        int value = expr_->evaluate();

        Variable::set_value(var_name_, value);

        return value;
    }

    const std::string& get_var_name() const
    {
        return var_name_;
    }
};

class PrintStmt : public Statement
{
private:
    std::unique_ptr<Expression> expr_;

public:
    PrintStmt(std::unique_ptr<Expression> expr) : expr_(std::move(expr))
    {
    }

    int evaluate() override
    {

        int value = expr_->evaluate();

        std::cout << value << std::endl;

        return value;
    }
};

class WhileStmt : public Statement
{
private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Statement> body_;

public:
    WhileStmt(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> body)
        : condition_(std::move(cond)), body_(std::move(body))
    {
    }

    int evaluate() override
    {
        while (condition_->evaluate() != 0)
        {
            body_->evaluate();
        }
        return 0;
    }
};

class IfStmt : public Statement
{
private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Statement> then_branch_;
    std::unique_ptr<Statement> else_branch_;

public:
    IfStmt(std::unique_ptr<Expression> cond,
           std::unique_ptr<Statement> then_branch,
           std::unique_ptr<Statement> else_branch = nullptr)
        : condition_(std::move(cond)), then_branch_(std::move(then_branch)),
          else_branch_(std::move(else_branch))
    {
    }

    int evaluate() override
    {
        if (condition_->evaluate() != 0)
        {
            then_branch_->evaluate();
        }
        else if (else_branch_)
        {
            else_branch_->evaluate();
        }
        return 0;
    }
};

class BlockStmt : public Statement
{
private:
    std::vector<std::unique_ptr<Statement>> statements_;

public:
    void add_statement(std::unique_ptr<Statement> stmt)
    {
        statements_.push_back(std::move(stmt));
    }

    int evaluate() override
    {
        for (auto& stmt : statements_)
        {
            stmt->evaluate();
        }
        return 0;
    }
};

} // namespace language

std::unordered_map<std::string, int> language::Variable::memory_;

#endif
