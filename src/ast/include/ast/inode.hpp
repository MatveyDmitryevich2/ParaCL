#ifndef INODE_HPP
#define INODE_HPP

#include "ast/fwd.hpp"
#include "error/error.hpp"
#include "visitor/visitor_fwd.hpp"

namespace language
{
class Interpreter;

class INode
{
private:
    SourceRange range_;

public:
    virtual ~INode() = default;
    virtual void evaluate(Interpreter& interp) = 0;
    virtual void accept(ASTVisitor* visitor) = 0;
    void set_range(SourceRange r)
    {
        range_ = std::move(r);
    }
    const SourceRange& range() const
    {
        return range_;
    }
};

class IExpression : public INode
{
};

class IStatement : public INode
{
};
} // namespace language

#endif // INODE_HPP
