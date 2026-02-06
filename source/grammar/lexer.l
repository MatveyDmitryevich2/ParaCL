%{
#include "parser.tab.hpp"
#include <iostream>
#include <string>
%}

%option noyywrap
%option yylineno

%%

[0-9]+          {
    yylval.number = std::stoi(yytext);
    std::cout << "LEXER: NUMBER '" << yytext << "'" << std::endl;
    return NUMBER;
}


"while"         { std::cout << "LEXER: WHILE"  << std::endl; return WHILE; }
"if"            { std::cout << "LEXER: IF"     << std::endl; return IF;    }
"print"         { std::cout << "LEXER: PRINT"  << std::endl; return PRINT; }
"?"             { std::cout << "LEXER: SACANF" << std::endl; return SCANF; }


[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.string = new std::string(yytext);
    std::cout << "LEXER: VAR '" << yytext << "'" << std::endl;
    return VAR;
}

[ \t\r\n]+           { /* Skip space and tabs*/ }


"+"             { std::cout << "LEXER: '+'" << std::endl; return '+';  }
"-"             { std::cout << "LEXER: '-'" << std::endl; return '-';  }
"*"             { std::cout << "LEXER: '*'" << std::endl; return '*';  }
"/"             { std::cout << "LEXER: '/'" << std::endl; return '/';  }

"("             { std::cout << "LEXER: '('" << std::endl; return '(';  }
")"             { std::cout << "LEXER: ')'" << std::endl; return ')';  }
"{"             { std::cout << "LEXER: '{'" << std::endl; return '{';  }
"}"             { std::cout << "LEXER: '}'" << std::endl; return '}';  }

"=="            { std::cout << "LEXER: '=='" << std::endl; return EQ;  }
"!="            { std::cout << "LEXER: '!='" << std::endl; return NE;  }
"<="            { std::cout << "LEXER: '<='" << std::endl; return LE;  }
">="            { std::cout << "LEXER: '>='" << std::endl; return GE;  }

"<"             { std::cout << "LEXER: '<'"  << std::endl; return L;   }
">"             { std::cout << "LEXER: '>'"  << std::endl; return G;   }

"&&"            { std::cout << "LEXER: '&&'" << std::endl; return AND; }
"||"            { std::cout << "LEXER: '||'" << std::endl; return OR;  }
"!"             { std::cout << "LEXER: '!'"  << std::endl; return NOT; }

"="             { std::cout << "LEXER: '='"  << std::endl; return '='; }
";"             { std::cout << "LEXER: ';'"  << std::endl; return ';'; }


.               {

    return yytext[0];
}
%%
