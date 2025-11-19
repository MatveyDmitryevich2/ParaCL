%option c++
%option noyywrap

%{
    #include <iostream>
    using std::cout;
    using std::endl;

    #define PRINTORRETURN(a) cout << a << " <" << yytext << ">" << endl; return 1;
%}

WS      [ \t\v]+
NL      [\n]+

VAR     [A-Za-z_][A-Za-z0-9_]*

%%

"0"       { PRINTORRETURN("NUMBER") }
[1-9][0-9]* { PRINTORRETURN("NUMBER") }

{WS}    /* ничего                 */
{NL}    /* для работы с ошибками  */
"="     { PRINTORRETURN("=")       }
"+"     { PRINTORRETURN("+")       }
"-"     { PRINTORRETURN("-")       }
"/"     { PRINTORRETURN("/")       }
"*"     { PRINTORRETURN("*")       }
"{"     { PRINTORRETURN("{")       }
"}"     { PRINTORRETURN("}")       }
"("     { PRINTORRETURN("(")       }
")"     { PRINTORRETURN(")")       }
"=="    { PRINTORRETURN("==")      }
"!="    { PRINTORRETURN("!=")      }
"<="    { PRINTORRETURN("<=")      }
">="    { PRINTORRETURN(">=")      }
"<"     { PRINTORRETURN("<")       }
">"     { PRINTORRETURN(">")       }
"!"     { PRINTORRETURN("!")       }
"&&"    { PRINTORRETURN("&&")      }
"||"    { PRINTORRETURN("||")      }
";"     { PRINTORRETURN(";")       }
"while" { PRINTORRETURN("WHILE")   }
"if"    { PRINTORRETURN("IF")      }
"else"  { PRINTORRETURN("ELSE")    }
"print" { PRINTORRETURN("PRINT")   }
"?"     { PRINTORRETURN("SCAN")    }
{VAR}   { PRINTORRETURN("VAR")     }
.       { PRINTORRETURN("UNKNOWN") }

%%
