#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "error/error_print.hpp"

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"

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
    case DiagnosticKind::Semantic:
        return "semantic error";
    default:
        return "error";
    }
}

const char* KindToColor(DiagnosticKind kind)
{
    switch (kind)
    {
    case DiagnosticKind::Lexical:
        return MAGENTA;
    case DiagnosticKind::Syntax:
        return YELLOW;
    case DiagnosticKind::Semantic:
        return CYAN;
    case DiagnosticKind::Runtime:
        return RED;
    case DiagnosticKind::Internal:
        return RED BOLD;
    default:
        return RESET;
    }
}
} // namespace

void PrintDiagnostic(const Diagnostic& diagnostic, const std::string& filename)
{

    std::cout << KindToColor(diagnostic.kind) << KindToString(diagnostic.kind)
              << RESET << ": " << diagnostic.message << '\n';

    if (diagnostic.range.has_value())
    {
        const SourceRange& range = diagnostic.range.value();

        std::cout << RED << " --> " << RESET << filename << ":"
                  << range.begin.line << ":" << range.begin.column << '\n';

        std::string line_text = diagnostic.line_text;

        if (line_text.empty() && !filename.empty())
        {
            std::ifstream file(filename);
            if (file.is_open())
            {
                std::string line;
                for (int i = 1; i <= range.begin.line; ++i)
                {
                    if (!std::getline(file, line))
                    {
                        line = "";
                        break;
                    }
                }
                line_text = std::move(line);
            }
        }

        if (!line_text.empty())
        {
            std::cout << "  " << RED << "|" << RESET << "\n";
            std::cout << RED << range.begin.line << " | " << RESET << line_text
                      << '\n';
            std::cout << "  " << RED << "|" << RESET << " ";

            for (int i = 1; i < range.begin.column; ++i)
            {
                std::cout << ' ';
            }

            int width = range.end.column - range.begin.column;
            if (width <= 0)
            {
                width = 1;
            }

            std::cout << RED;
            for (int i = 0; i < width; ++i)
            {
                std::cout << '^';
            }
            std::cout << RESET << '\n';
        }
    }

    for (const std::string& add_message : diagnostic.add_message)
    {
        std::cout << "  " << RED << "=" << RESET << " " << add_message << '\n';
    }
}
