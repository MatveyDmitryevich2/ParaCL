#include "driver.hpp"
#include <iostream>

extern int yyparse(language::Driver* driver);

int main()
{
    language::Driver driver;

    std::cout << "ParaCL Parser\n";
    std::cout << "Enter program (Ctrl+D to finish):\n\n";

    if (yyparse(&driver) == 0)
    {
        std::cout << "\n AST built successfully!\n";
        std::cout
            << "(Tree is stored in driver, ready for further processing)\n";
    }
    else
    {
        std::cerr << "\n Parsing failed\n";
    }

    return 0;
}
