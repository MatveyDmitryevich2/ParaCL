

#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include "node.hpp"

namespace
{

    std::unordered_map<std::string, int> variable_memory;

    int get_variable_value(const std::string& name)
    {

        if (variable_memory.find(name) == variable_memory.end())
        {
            variable_memory[name] = 0;
        }
        return variable_memory[name];
    }

    void set_variable_value(const std::string& name, int value) {
        variable_memory[name] = value;
    }

    void clear_variables() {
        variable_memory.clear();
    }
}

language::Number::Number(int value) : value_(value) {}

int language::Number::evaluate() {
    return value_;
}

int language::Number::get_value() const {
    return value_;
}


language::Variable::Variable(const std::string& name) : name_(name) {}

int language::Variable::evaluate() {
    return get_variable_value(name_);
}

const std::string& language::Variable::get_name() const {
    return name_;
}


language::BinaryOp::BinaryOp(Op op, std::unique_ptr<IExpression> left,
                             std::unique_ptr<IExpression> right)
    : op_(op), left_(std::move(left)), right_(std::move(right)) {}

int language::BinaryOp::evaluate() {
    int left_val = left_->evaluate();
    int right_val = right_->evaluate();

    switch(op_) {
        case Op::ADD: return left_val + right_val;
        case Op::SUB: return left_val - right_val;
        case Op::MUL: return left_val * right_val;
        case Op::DIV:
            if (right_val == 0) {
                throw std::runtime_error("Division by zero!");
            }
            return left_val / right_val;
        default: return 0;
    }
}

language::BinaryOp::Op language::BinaryOp::get_op() const {
    return op_;
}

language::IExpression* language::BinaryOp::get_left() const {
    return left_.get();
}

language::IExpression* language::BinaryOp::get_right() const {
    return right_.get();
}


language::Assignment::Assignment(const std::string& var_name,
                                 std::unique_ptr<IExpression> expr)
    : var_name_(var_name), expr_(std::move(expr)) {}

int language::Assignment::evaluate() {
    int value = expr_->evaluate();
    set_variable_value(var_name_, value);
    return value;
}

const std::string& language::Assignment::get_var_name() const {
    return var_name_;
}

language::IExpression* language::Assignment::get_expr() const {
    return expr_.get();
}


language::PrintStmt::PrintStmt(std::unique_ptr<IExpression> expr)
    : expr_(std::move(expr)) {}

int language::PrintStmt::evaluate() {
    int value = expr_->evaluate();
    std::cout << value << std::endl;
    return value;
}

language::IExpression* language::PrintStmt::get_expr() const {
    return expr_.get();
}

language::BlockStmt::BlockStmt() = default;

void language::BlockStmt::add_statement(std::unique_ptr<IStatement> stmt) {
    statements_.push_back(std::move(stmt));
}

int language::BlockStmt::evaluate() {
    for (auto& stmt : statements_) {
        stmt->evaluate();
    }
    return 0;
}

size_t language::BlockStmt::get_statement_count() const {
    return statements_.size();
}

language::IStatement* language::BlockStmt::get_statement(size_t index) const {
    return statements_[index].get();
}
