#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>

class VariableTable
{
private:
    std::unordered_map<std::string, int> hash;
    using ItHash = std::unordered_map<std::string, int>::iterator;

public:
    bool IsSuchVariable(const std::string& name_variable) const
    {
        return hash.find(name_variable) != hash.end();
    }

    bool AddNewVariable(const std::string& name_variable, int value)
    {
        return hash.emplace(name_variable, value).second;
    }

    bool RewriteOldVariable(const std::string& name_variable, int value)
    {
        ItHash it = hash.find(name_variable);
        if (it == hash.end()) { return false; }
        it->second = value;
        return true;
        // if (!IsSuchVariable(name_variable)) { return false; }
        // hash.insert_or_assign(name_variable, value);
        // return true;
    }

    int GetValue(const std::string& name) const
    {
        return hash.at(name);
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
        stack.pop_back();
    }

    void AddVariable(const std::string& name_variable, int value)
    {
        VariableTable& top = stack.back();
        if (!top.AddNewVariable(name_variable, value))
            throw std::runtime_error("Variable redefinition: " + name_variable);
    }


    void WriteNewValueVar(const std::string& name_variable, int value)
    {
        for(size_t i = stack.size(); i > 0; --i)
        {
            VariableTable& current_table = stack[i - 1];
            if (current_table.RewriteOldVariable(name_variable, value)) { return; }
        }
        throw std::runtime_error("Variable not found: " + name_variable);
    }

    int GetValueVariable(const std::string& name_variable) const
    {
        for(size_t i = stack.size(); i > 0; --i)
        {
            const VariableTable& current_table = stack[i - 1];
            if (current_table.IsSuchVariable(name_variable))
                return current_table.GetValue(name_variable);
        }
        throw std::runtime_error("Variable not found: " + name_variable);
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
            throw std::runtime_error("PopValue on empty stack");
        int top_value = stack.back();
        stack.pop_back();
        return top_value;
    }

    void PushValue(int value)
    {
        stack.push_back(value);
    }

    int SeeTop() const
    {
        if (stack.empty())
            throw std::runtime_error("SeeTop on empty stack");
        return stack.back();
    }
};

class Interpreter
{
    
};