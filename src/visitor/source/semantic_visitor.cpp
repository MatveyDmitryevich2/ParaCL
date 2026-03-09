#include "visitor/semantic_visitor.hpp"
#include "ast/node.hpp"
#include "visitor/ast_visitor.hpp"

void SemanticVisitor::analyze(language::BlockStmt* root)
{

    while (!scopes_.empty())
        scopes_.pop();
    errors_.clear();

    scopes_.push({});

    if (root)
    {
        root->accept(this);
    }
}

void SemanticVisitor::visit(language::BlockStmt* node)
{
    if (!node)
        return;

    scopes_.push({});

    for (size_t i = 0; i < node->get_statement_count(); ++i)
    {
        node->get_statement(i)->accept(this);
    }

    scopes_.pop();
}

void SemanticVisitor::visit(language::Assignment* node)
{
    if (!node)
        return;

    if (auto* expr = node->get_expr())
    {
        expr->accept(this);
    }

    scopes_.top().insert(node->get_var_name());
}

void SemanticVisitor::visit(language::PrintStmt* node)
{
    if (!node)
        return;

    if (auto* expr = node->get_expr())
    {
        expr->accept(this);
    }
}

void SemanticVisitor::visit(language::WhileStmt* node)
{
    if (!node)
        return;

    if (auto* cond = node->get_condition())
    {
        cond->accept(this);
    }

    if (auto* body = node->get_body())
    {
        body->accept(this);
    }
}

void SemanticVisitor::visit(language::IfStmt* node)
{
    if (!node)
        return;

    if (auto* cond = node->get_condition())
    {
        cond->accept(this);
    }

    if (auto* then_branch = node->get_body_if())
    {
        then_branch->accept(this);
    }

    if (auto* else_branch = node->get_body_else())
    {
        else_branch->accept(this);
    }
}

void SemanticVisitor::visit(language::BinaryOp* node)
{
    if (!node)
        return;

    if (auto* left = node->get_left())
    {
        left->accept(this);
    }

    if (auto* right = node->get_right())
    {
        right->accept(this);
    }
}

void SemanticVisitor::visit(language::UnaryOp* node)
{
    if (!node)
        return;

    if (auto* expr = node->get_expr())
    {
        expr->accept(this);
    }
}

void SemanticVisitor::visit(language::Variable* node)
{
    if (!node)
        return;

    std::string name = node->get_name();

    if (!isDeclared(name))
    {
        addError("variable not found: " + name, node);
    }
}

void SemanticVisitor::visit(language::Number* node)
{

    (void)node;
}

void SemanticVisitor::visit(language::ScanfExpr* node)
{

    (void)node;
}

void SemanticVisitor::visit(language::ExpressionStmt* node)
{
    if (!node)
        return;

    if (auto* expr = node->get_expr())
    {
        expr->accept(this);
    }
}

bool SemanticVisitor::isDeclared(const std::string& name) const
{
    auto scopes_copy = scopes_;
    while (!scopes_copy.empty())
    {
        if (scopes_copy.top().count(name))
        {
            return true;
        }
        scopes_copy.pop();
    }
    return false;
}

void SemanticVisitor::addError(const std::string& message,
                               const language::INode* node)
{
    Diagnostic diagnostic;
    diagnostic.kind = DiagnosticKind::Semantic;
    diagnostic.message = message;

    if (node)
    {
        diagnostic.range = node->range();
    }

    errors_.emplace_back(std::move(diagnostic));
}
