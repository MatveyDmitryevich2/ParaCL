%{
#include "parser.tab.hpp"
#include <iostream>
#include <string>
%}

%option noyywrap

%%

[0-9]+          {
    yylval.number = std::stoi(yytext);
    std::cout << "LEXER: NUMBER '" << yytext << "'" << std::endl;
    return NUMBER;
}


"while"         { std::cout << "LEXER: WHILE" << std::endl; return WHILE; }
"if"            { std::cout << "LEXER: IF" << std::endl; return IF; }
"print"         { std::cout << "LEXER: PRINT" << std::endl; return PRINT; }

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.string = new std::string(yytext);
    std::cout << "LEXER: VAR '" << yytext << "'" << std::endl;
    return VAR;
}

[ \t\n]           { /* Skip space and tabs*/ }


"+"             { std::cout << "LEXER: '+'" << std::endl; return '+'; }
"-"             { std::cout << "LEXER: '-'" << std::endl; return '-'; }
"*"             { std::cout << "LEXER: '*'" << std::endl; return '*'; }
"/"             { std::cout << "LEXER: '/'" << std::endl; return '/'; }

"("             { std::cout << "LEXER: '('" << std::endl; return '('; }
")"             { std::cout << "LEXER: ')'" << std::endl; return ')'; }
"{"             { std::cout << "LEXER: '{'" << std::endl; return '{'; }
"}"             { std::cout << "LEXER: '}'" << std::endl; return '}'; }

"="             { std::cout << "LEXER: '='" << std::endl; return '='; }
";"             { std::cout << "LEXER: ';'" << std::endl; return ';'; }


.               {

    return yytext[0];
}
%%
