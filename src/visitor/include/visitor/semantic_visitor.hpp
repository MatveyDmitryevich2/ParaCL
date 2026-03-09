#ifndef SEMANTIC_VISITOR_HPP
#define SEMANTIC_VISITOR_HPP

#include "error/error.hpp"
#include "visitor/ast_visitor.hpp"
#include <stack>
#include <unordered_set>
#include <vector>

namespace language
{
class INode;
}

class SemanticVisitor : public ASTVisitor
{
public:
    void visit(language::BlockStmt* node) override;
    void visit(language::Assignment* node) override;
    void visit(language::PrintStmt* node) override;
    void visit(language::WhileStmt* node) override;
    void visit(language::IfStmt* node) override;
    void visit(language::BinaryOp* node) override;
    void visit(language::UnaryOp* node) override;
    void visit(language::Variable* node) override;
    void visit(language::Number* node) override;
    void visit(language::ScanfExpr* node) override;
    void visit(language::ExpressionStmt* node) override;

    const std::vector<SemanticError>& get_errors() const
    {
        return errors_;
    }
    void clear_errors()
    {
        errors_.clear();
    }
    void analyze(language::BlockStmt* root);

private:
    std::stack<std::unordered_set<std::string>> scopes_;
    std::vector<SemanticError> errors_;

    bool isDeclared(const std::string& name) const;
    void addError(const std::string& message, const language::INode* node);
};

#endif
