#include <iostream>
#include <string>

#include "error/error_print.hpp"

namespace
{
const char* KindToString(DiagnosticKind kind)
{
    switch (kind)
    {
    case DiagnosticKind::Lexical:
        return "lexical error";
    case DiagnosticKind::Syntax:
        return "syntax error";
    case DiagnosticKind::Runtime:
        return "runtime error";
    case DiagnosticKind::Internal:
        return "internal error";
    default:
        return "error";
    }
}
} // namespace

void PrintDiagnostic(const Diagnostic& diagnostic,
                     const std::string& filename)
{
    std::cout << KindToString(diagnostic.kind) << ": "
        << diagnostic.message << '\n';

    if (diagnostic.range.has_value())
    {
        const SourceRange& range = diagnostic.range.value();

        std::cout << " --> " << filename
            << ":" << range.begin.line
            << ":" << range.begin.column << '\n';

        if (!diagnostic.line_text.empty())
        {
            std::cout << "  |\n";
            std::cout << range.begin.line << " | " << diagnostic.line_text << '\n';
            std::cout << "  | ";

            for (int i = 1; i < range.begin.column; ++i)
            {
                std::cout << ' ';
            }

            int width = range.end.column - range.begin.column;
            if (width <= 0)
            {
                width = 1;
            }

            for (int i = 0; i < width; ++i)
            {
                std::cout << '^';
            }

            std::cout << '\n';
        }
    }

    for (const std::string& add_message : diagnostic.add_message)
    {
        std::cout << "  = add_message: " << add_message << '\n';
    }
}