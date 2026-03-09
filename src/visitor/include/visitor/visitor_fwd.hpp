
#ifndef VISITOR_FWD_HPP
#define VISITOR_FWD_HPP

namespace language
{
class BlockStmt;
class Assignment;
class PrintStmt;
class WhileStmt;
class IfStmt;
class BinaryOp;
class UnaryOp;
class Variable;
class Number;
class ScanfExpr;
class ExpressionStmt;
} // namespace language

class ASTVisitor
{
public:
    virtual ~ASTVisitor() = default;

    virtual void visit(language::BlockStmt* node) = 0;
    virtual void visit(language::Assignment* node) = 0;
    virtual void visit(language::PrintStmt* node) = 0;
    virtual void visit(language::WhileStmt* node) = 0;
    virtual void visit(language::IfStmt* node) = 0;
    virtual void visit(language::BinaryOp* node) = 0;
    virtual void visit(language::UnaryOp* node) = 0;
    virtual void visit(language::Variable* node) = 0;
    virtual void visit(language::Number* node) = 0;
    virtual void visit(language::ScanfExpr* node) = 0;
    virtual void visit(language::ExpressionStmt* node) = 0;
};

#endif // VISITOR_FWD_HPP
