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
   // std::cout << "LEXER: NUMBER '" << yytext << "'" << std::endl;
    return NUMBER;
}


"while"         { return WHILE; }
"if"            { return IF;    }
"print"         { return PRINT; }
"?"             { return SCANF; }


[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.string = new std::string(yytext);
    //std::cout << "LEXER: VAR '" << yytext << "'" << std::endl;
    return VAR;
}

[ \t\r\n]+      { /* Skip space and tabs*/ }


"+"             {  return PLUS;  }
"-"             {  return MINUS;  }
"*"             {  return MUL;  }
"/"             {  return DIV;  }

"("             {  return '(';  }
")"             {  return ')';  }
"{"             {  return '{';  }
"}"             {  return '}';  }

"=="            { return EQ;  }
"!="            { return NE;  }
"<="            { return LE;  }
">="            { return GE;  }

"<"             { return L;   }
">"             { return G;   }

"&&"            { return AND; }
"||"            { return OR;  }
"!"             { return NOT; }

"="             { return '='; }
";"             { return ';'; }


.               {

    return yytext[0];
}
%%
