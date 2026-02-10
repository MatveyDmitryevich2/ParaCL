#include <iostream>

extern int yyparse();

int main()
{
    std::cout << "ParaCL Parser" << std::endl;
    std::cout << "Enter statements (empty line to exit):" << std::endl;

    int result = yyparse();

    if (result == 0)
    {
        std::cout << "Parsing completed successfully" << std::endl;
    }
    else
    {
        std::cout << "Parsing failed with error" << std::endl;
    }

    return result;
}
