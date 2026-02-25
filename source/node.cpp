#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "interpreter.hpp"
#include "node.hpp"

void language::Number::evaluate(Interpreter& interp)
{
    interp.eval_stack.PushValue(value_);
}

void language::Variable::evaluate(Interpreter& interp)
{
    interp.eval_stack.PushValue(interp.scope_stack.GetValueVariable(name_));
}

void language::BinaryOp::evaluate(Interpreter& interp)
{
    left_->evaluate(interp);
    right_->evaluate(interp);
    int right_val = interp.eval_stack.PopValue();
    int left_val = interp.eval_stack.PopValue();

    int res = 0;

    switch (op_)
    {
    case Op::OR:
        res = (left_val != 0) || (right_val != 0) ? 1 : 0;
        break;
    case Op::AND:
        res = (left_val != 0) && (right_val != 0) ? 1 : 0;
        break;
    case Op::EQ:
        res = (left_val == right_val) ? 1 : 0;
        break;
    case Op::NE:
        res = (left_val != right_val) ? 1 : 0;
        break;
    case Op::L:
        res = (left_val < right_val) ? 1 : 0;
        break;
    case Op::G:
        res = (left_val > right_val) ? 1 : 0;
        break;
    case Op::LE:
        res = (left_val <= right_val) ? 1 : 0;
        break;
    case Op::GE:
        res = (left_val >= right_val) ? 1 : 0;
        break;
    case Op::ADD:
        res = left_val + right_val;
        break;
    case Op::SUB:
        res = left_val - right_val;
        break;
    case Op::MUL:
        res = left_val * right_val;
        break;
    case Op::DIV:

        if (right_val == 0)
        {
            throw std::runtime_error("Division by zero!");
        }
        res = left_val / right_val;
        break;

    default:
        throw std::runtime_error("Unknown binary operator");
    }

    interp.eval_stack.PushValue(res);
}

void language::UnaryOp::evaluate(Interpreter& interp)
{
    expr_->evaluate(interp);

    int val = interp.eval_stack.PopValue();

    switch (op_)
    {
    case Op::MINUS:
        interp.eval_stack.PushValue(-val);
        break;
    case Op::NOT:
        interp.eval_stack.PushValue((val == 0) ? 1 : 0);
        break;
    default:
        throw std::runtime_error("Unknown unary operator");
    }
}

void language::Assignment::evaluate(Interpreter& interp)
{
    expr_->evaluate(interp);
    int value = interp.eval_stack.PopValue();

    interp.scope_stack.AssignOrDeclareCurrent(var_name_, value);

    interp.eval_stack.PushValue(value);
}

void language::ExpressionStmt::evaluate(Interpreter& interp)
{
    expr_->evaluate(interp);
    if (!interp.eval_stack.IsEmpty())
    {
        interp.eval_stack.PopValue();
    }
}

void language::PrintStmt::evaluate(Interpreter& interp)
{
    expr_->evaluate(interp);
    std::cout << interp.eval_stack.PopValue() << '\n';
}

void language::ScanfExpr::evaluate(Interpreter& interp)
{
    int value = 0;

    // Читаем из потока интерпретатора
    if (fscanf(interp.getInputStream(), "%d", &value) != 1)
    {
        throw std::runtime_error("Failed to read input");
    }

    interp.eval_stack.PushValue(value);
}

void language::IfStmt::evaluate(Interpreter& interp)
{
    condition_->evaluate(interp);
    if (interp.eval_stack.PopValue())
        body_if_->evaluate(interp);
    else if (body_else_)
        body_else_->evaluate(interp);
}

void language::WhileStmt::evaluate(Interpreter& interp)
{
    while (true)
    {
        condition_->evaluate(interp);
        const int cond = interp.eval_stack.PopValue();

        if (!cond)
            break;

        body_->evaluate(interp);
    }
}

void language::BlockStmt::evaluate(Interpreter& interp)
{
    interp.scope_stack.AddScope();
    for (size_t i = 0; i < statements_.size(); ++i)
    {
        statements_[i]->evaluate(interp);
    }
    interp.scope_stack.DeleteScope();
}

void language::Interpreter::Run(BlockStmt& root)
{
    root.evaluate(*this);
}
