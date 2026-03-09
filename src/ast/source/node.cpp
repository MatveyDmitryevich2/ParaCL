#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "ast/node.hpp"
#include "error/error.hpp"
#include "interpreter/interpreter.hpp"
#include "visitor/visitor_fwd.hpp"

void language::Number::evaluate(Interpreter& interp)
{
    interp.eval_stack.PushValue(value_);
}

void language::Variable::evaluate(Interpreter& interp)
{
    try
    {
        interp.eval_stack.PushValue(interp.scope_stack.GetValueVariable(name_));
    }
    catch (const RuntimeDiagError&)
    {
        throw;
    }
    catch (const std::exception&)
    {
        Diagnostic diagnostic;
        diagnostic.kind = DiagnosticKind::Runtime;
        diagnostic.message = "variable '" + name_ + "' is not defined";
        diagnostic.range = range();
        diagnostic.add_message.push_back(
            "assign a value to the variable before using it");
        throw RuntimeDiagError(std::move(diagnostic));
    }
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
    case Op::MOD:

        if (right_val == 0)
        {
            Diagnostic diagnostic;
            diagnostic.kind = DiagnosticKind::Runtime;
            diagnostic.message = "modulo by zero";
            diagnostic.range = range();
            diagnostic.add_message.push_back("right operand evaluated to 0");
            throw RuntimeDiagError(std::move(diagnostic));
        }
        res = left_val % right_val;
        break;

    case Op::DIV:

        if (right_val == 0)
        {
            Diagnostic diagnostic;
            diagnostic.kind = DiagnosticKind::Runtime;
            diagnostic.message = "division by zero";
            diagnostic.range = range();
            diagnostic.add_message.push_back("right operand evaluated to 0");
            throw RuntimeDiagError(std::move(diagnostic));
        }
        res = left_val / right_val;
        break;

    default:
        Diagnostic diagnostic;
        diagnostic.kind = DiagnosticKind::Internal;
        diagnostic.message = "unknown binary operator";
        throw InternalError(std::move(diagnostic));
    }

    interp.eval_stack.PushValue(res);
}

void language::UnaryOp::evaluate(Interpreter& interp)
{
    expr_->evaluate(interp);

    int val = interp.eval_stack.PopValue();

    switch (op_)
    {
    case Op::UMINUS:
        interp.eval_stack.PushValue(-val);
        break;
    case Op::NOT:
        interp.eval_stack.PushValue((val == 0) ? 1 : 0);
        break;
    default:
        Diagnostic diagnostic;
        diagnostic.kind = DiagnosticKind::Internal;
        diagnostic.message = "unknown unary operator";
        throw InternalError(std::move(diagnostic));
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

    if (fscanf(interp.getInputStream(), "%d", &value) != 1)
    {
        Diagnostic diagnostic;
        diagnostic.kind = DiagnosticKind::Runtime;
        diagnostic.message = "failed to read integer from input";
        diagnostic.range = range();
        throw RuntimeDiagError(std::move(diagnostic));
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

void language::Number::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::Variable::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::BinaryOp::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::UnaryOp::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::Assignment::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::ExpressionStmt::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::PrintStmt::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::ScanfExpr::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::IfStmt::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::WhileStmt::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}

void language::BlockStmt::accept(ASTVisitor* visitor)
{
    visitor->visit(this);
}
