#ifndef ERROR_HPP
#define ERROR_HPP

#include <optional>
#include <string>
#include <vector>

enum class DiagnosticKind
{
    Lexical,
    Syntax,
    Runtime,
    Semantic,
    Internal,
};

struct SourcePosition
{
    int line;
    int column;
};

struct SourceRange
{
    SourcePosition begin;
    SourcePosition end;
};

struct Diagnostic
{
    DiagnosticKind kind;
    std::string message;
    std::optional<SourceRange> range;
    std::string line_text;
    std::vector<std::string> add_message;
};

class DiagnosticError : public std::exception
{
private:
    Diagnostic diagnostic_;

public:
    explicit DiagnosticError(Diagnostic diagnostic)
        : diagnostic_(std::move(diagnostic))
    {
    }

    const Diagnostic& diagnostic() const noexcept
    {
        return diagnostic_;
    }
};

class SemanticError : public DiagnosticError
{
public:
    explicit SemanticError(Diagnostic diagnostic)
        : DiagnosticError(std::move(diagnostic))
    {
    }
};

class LexicalError : public DiagnosticError
{
public:
    explicit LexicalError(Diagnostic diagnostic)
        : DiagnosticError(std::move(diagnostic))
    {
    }
};

class ParseError : public DiagnosticError
{
public:
    explicit ParseError(Diagnostic diagnostic)
        : DiagnosticError(std::move(diagnostic))
    {
    }
};

class RuntimeDiagError : public DiagnosticError
{
public:
    explicit RuntimeDiagError(Diagnostic diagnostic)
        : DiagnosticError(std::move(diagnostic))
    {
    }
};

class InternalError : public DiagnosticError
{
public:
    explicit InternalError(Diagnostic diagnostic)
        : DiagnosticError(std::move(diagnostic))
    {
    }
};

#endif // ERROR_HPP
