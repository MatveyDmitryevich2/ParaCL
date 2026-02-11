#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "node.hpp"
// ================ Symbol table ============

static std::unordered_map<std::string, int> variable_memory;

static int get_variable_value(const std::string& name)
{
    if (variable_memory.find(name) == variable_memory.end())
    {
        variable_memory[name] = 0;
    }
    return variable_memory[name];
}

static void set_variable_value(const std::string& name, int value)
{
    variable_memory[name] = value;
}
// ============ Number ============

language::Number::Number(int value) : value_(value)
{
}

int language::Number::evaluate()
{
    return value_;
}

int language::Number::get_value() const
{
    return value_;
}

// ============ Variable ============

language::Variable::Variable(const std::string& name) : name_(name)
{
}

int language::Variable::evaluate()
{
    return get_variable_value(name_);
}

const std::string& language::Variable::get_name() const
{
    return name_;
}

// ============ BinaryOp ============

language::BinaryOp::BinaryOp(Op op, std::unique_ptr<IExpression> left,
                             std::unique_ptr<IExpression> right)
    : op_(op), left_(std::move(left)), right_(std::move(right))
{
}

int language::BinaryOp::evaluate()
{
    int left_val = left_->evaluate();
    int right_val = right_->evaluate();

    switch (op_)
    {
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

    case Op::OR:
        return (left_val != 0) || (right_val != 0) ? 1 : 0;
    case Op::AND:
        return (left_val != 0) && (right_val != 0) ? 1 : 0;

    case Op::EQ:
        return (left_val == right_val) ? 1 : 0;
    case Op::NE:
        return (left_val != right_val) ? 1 : 0;
    case Op::L:
        return (left_val < right_val) ? 1 : 0; // L = Less
    case Op::G:
        return (left_val > right_val) ? 1 : 0; // G = Greater
    case Op::LE:
        return (left_val <= right_val) ? 1 : 0; // LE = Less or Equal
    case Op::GE:
        return (left_val >= right_val) ? 1 : 0; // GE = Greater or Equal

    default:
        throw std::runtime_error("Unknown binary operator");
    }
}

language::BinaryOp::Op language::BinaryOp::get_op() const
{
    return op_;
}

language::IExpression* language::BinaryOp::get_left() const
{
    return left_.get();
}

language::IExpression* language::BinaryOp::get_right() const
{
    return right_.get();
}

// ============ Assignment ============

language::Assignment::Assignment(const std::string& var_name,
                                 std::unique_ptr<IExpression> expr)
    : var_name_(var_name), expr_(std::move(expr))
{
}

int language::Assignment::evaluate()
{
    int value = expr_->evaluate();
    set_variable_value(var_name_, value);
    return value;
}

const std::string& language::Assignment::get_var_name() const
{
    return var_name_;
}

language::IExpression* language::Assignment::get_expr() const
{
    return expr_.get();
}

// ============ PrintStmt ============

language::PrintStmt::PrintStmt(std::unique_ptr<IExpression> expr)
    : expr_(std::move(expr))
{
}

int language::PrintStmt::evaluate()
{
    int value = expr_->evaluate();
    std::cout << value << std::endl;
    return value;
}

language::IExpression* language::PrintStmt::get_expr() const
{
    return expr_.get();
}

// ============ ScanfExpr ============
language::ScanfExpr::ScanfExpr() = default;

int language::ScanfExpr::evaluate()
{
    int value = 0;
    std::cin >> value;

    if (std::cin.fail())
    {
        std::cin.clear();

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::runtime_error("Invalid input for '?'");
    }

    return value;
}

// ============ IfStmt ============
language::IfStmt::IfStmt(std::unique_ptr<IExpression> condition,
                         std::unique_ptr<IStatement> then_branch,
                         std::unique_ptr<IStatement> else_branch)
    : condition_(std::move(condition)), then_branch_(std::move(then_branch)),
      else_branch_(std::move(else_branch))
{
}

int language::IfStmt::evaluate()
{

    if (condition_->evaluate() != 0)
    {
        return then_branch_->evaluate();
    }
    else if (else_branch_)
    {
        return else_branch_->evaluate();
    }
    return 0;
}

language::IExpression* language::IfStmt::get_condition() const
{
    return condition_.get();
}

language::IStatement* language::IfStmt::get_then_branch() const
{
    return then_branch_.get();
}

language::IStatement* language::IfStmt::get_else_branch() const
{
    return else_branch_.get();
}

// ============ WhileStmt ============
language::WhileStmt::WhileStmt(std::unique_ptr<IExpression> condition,
                               std::unique_ptr<IStatement> body)
    : condition_(std::move(condition)), body_(std::move(body))
{
}

int language::WhileStmt::evaluate()
{
    while (condition_->evaluate() != 0)
    {
        body_->evaluate();
    }
    return 0;
}

language::IExpression* language::WhileStmt::get_condition() const
{
    return condition_.get();
}

language::IStatement* language::WhileStmt::get_body() const
{
    return body_.get();
}

// ============ BlockStmt ============

void language::BlockStmt::add_statement(std::unique_ptr<IStatement> stmt)
{
    statements_.push_back(std::move(stmt));
}

int language::BlockStmt::evaluate()
{
    for (auto& stmt : statements_)
    {
        stmt->evaluate();
    }
    return 0;
}
