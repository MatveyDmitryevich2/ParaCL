#include <iostream>
#include <string>

#include "ast/ast.hpp"
#include "ast/node.hpp"
#include "interpreter/interpreter.hpp"
#include "parser.tab.hpp"
#include "error/error.hpp"
#include "error/error_print.hpp"

extern FILE* yyin;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 0;
    }

    yyin = std::fopen(argv[1], "r");
    if (!yyin)
    {
        std::cerr << "Failed to open file: " << argv[1] << '\n';
        return 0;
    }

    try
    {
        language::AST ast;
        yy::parser parser(&ast);

        if (parser.parse() != 0)
        {
            std::fclose(yyin);
            return 0;
        }

        std::fclose(yyin);
        yyin = nullptr;

        language::Interpreter interp;
        interp.Run(*ast.get_root());
        return 0;
    }
    catch (const DiagnosticError& e)
    {
        if (yyin)
        {
            std::fclose(yyin);
            yyin = nullptr;
        }

        PrintDiagnostic(e.diagnostic(), argv[1]);
        return 0;
    }
    catch (const std::exception& e)
    {
        if (yyin)
        {
            std::fclose(yyin);
            yyin = nullptr;
        }

        Diagnostic diagnostic;
        diagnostic.kind = DiagnosticKind::Internal;
        diagnostic.message = e.what();
        PrintDiagnostic(diagnostic, argv[1]);
        return 0;
    }
}
