#include <iostream>

extern int yyparse();

int main()
{
    std::cout << "ParaCL Parser" << std::endl;
    return yyparse();
}
