#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/fwd.hpp"
#include "ast/node.hpp"
#include "error/error.hpp"

namespace language
{
using ValT = int;

class VariableTable
{
private:
    std::unordered_map<std::string, int> variables;
    using ItHash = std::unordered_map<std::string, int>::iterator;

public:
    bool IsVariableExist(const std::string& name_variable) const
    {
        return variables.find(name_variable) != variables.end();
    }

    bool AddNewVariable(const std::string& name_variable, int value)
    {
        return variables.emplace(name_variable, value).second;
    }

    bool Assign(const std::string& name_variable, int value)
    {
        ItHash it = variables.find(name_variable);
        if (it == variables.end())
        {
            return false;
        }
        it->second = value;
        return true;
    }

    int GetValue(const std::string& name) const
    {
        return variables.at(name);
    }
};

class ScopeStack
{
private:
    std::vector<VariableTable> stack;

public:
    void AddScope()
    {
        stack.emplace_back();
    }

    void DeleteScope()
    {
        if (stack.empty())
        {
            Diagnostic diagnostic;
            diagnostic.kind = DiagnosticKind::Internal;
            diagnostic.message = "DeleteScope on empty stack";
            throw InternalError(std::move(diagnostic));
        }

        stack.pop_back();
    }
    bool IsEmpty() const
    {
        return stack.empty();
    }
    void AddVariable(const std::string& name_variable, int value)
    {
        VariableTable& top = stack.back();
        if (!top.AddNewVariable(name_variable, value))
            throw std::runtime_error("Variable redefinition: " + name_variable);
    }

    void AssignOrDeclareCurrent(const std::string& name_variable, int value)
    {
        for (size_t i = stack.size(); i > 0; --i)
        {
            VariableTable& current_table = stack[i - 1];
            if (current_table.Assign(name_variable, value))
            {
                return;
            }
        }
        if (stack.empty())
            throw std::runtime_error("No scopes available for variable: " +
                                     name_variable);

        stack.back().AddNewVariable(name_variable, value);
    }

    int GetValueVariable(const std::string& name_variable) const
    {
        for (size_t i = stack.size(); i > 0; --i)
        {
            const VariableTable& current_table = stack[i - 1];
            if (current_table.IsVariableExist(name_variable))
                return current_table.GetValue(name_variable);
        }
        Diagnostic diagnostic;
        diagnostic.kind = DiagnosticKind::Runtime;
        diagnostic.message = "variable not found: " + name_variable;
        throw RuntimeDiagError(std::move(diagnostic));
    }
};

class EvaluationStack
{
private:
    std::vector<int> stack;

public:
    int PopValue()
    {
        if (stack.empty())
        {
            Diagnostic diagnostic;
            diagnostic.kind = DiagnosticKind::Internal;
            diagnostic.message = "PopValue on empty stack";
            throw InternalError(std::move(diagnostic));
        }

        int top_value = stack.back();
        stack.pop_back();
        return top_value;
    }

    bool IsEmpty() const
    {
        return stack.empty();
    }

    void PushValue(int value)
    {
        stack.push_back(value);
    }

    int SeeTop() const
    {
        if (stack.empty())
        {
            Diagnostic diagnostic;
            diagnostic.kind = DiagnosticKind::Internal;
            diagnostic.message = "SeeTop on empty stack";
            throw InternalError(std::move(diagnostic));
        }
        return stack.back();
    }
};

class Interpreter
{
private:
    FILE* input_stream_;

public:
    Interpreter(FILE* input = stdin) : input_stream_(input)
    {
    }

    FILE* getInputStream() const
    {
        return input_stream_;
    }

    ScopeStack scope_stack;
    EvaluationStack eval_stack;

    void Run(BlockStmt& root);
};
} // namespace language

#endif // INTERPRETER_HPP
