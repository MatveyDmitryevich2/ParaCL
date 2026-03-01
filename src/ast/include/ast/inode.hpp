#ifndef INODE_HPP
#define INODE_HPP

#include "fwd.hpp"

namespace language
{
class Interpreter;

class INode
{
public:
    virtual ~INode() = default;
    virtual void evaluate(Interpreter& interp) = 0;
};

class IExpression : public INode
{
};

class IStatement : public INode
{
};
} // namespace language

#endif // INODE_HPP