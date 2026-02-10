#ifndef INODE_HPP
#define INODE_HPP

#include <memory>

namespace language
{

class Number;
class Variable;
class BinaryOp;
class Assignment;
class PrintStmt;

class INode
{
public:
    virtual ~INode() = default;
    virtual int evaluate() = 0;

    class IExpression : public INode
    {
    };
    class IStatement : public INode
    {
    };
}
} // namespace language

#endif // INODE_HPP
