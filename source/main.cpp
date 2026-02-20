#include "ast.hpp"
#include "interpreter.hpp"
#include "node.hpp"
#include "parser.tab.hpp"

#include <iostream>
#include <string>

extern FILE* yyin;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 0;
    }

    yyin = std::fopen(argv[1], "r");

    language::AST ast;
    yy::parser parser(&ast);

    if (parser.parse() != 0)
    {
        std::cerr << "Parser ERROR \n";
        std::fclose(yyin);
        return 0;
    }

    std::fclose(yyin);

    language::Interpreter interp;

    try
    {
        interp.Run(*ast.get_root());
    }
    catch (const std::exception& e)
    {
        std::cerr << "Runtime error: " << e.what() << "\n";
    }

    return 0;
}
