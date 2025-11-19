%option c++

%{
    using std::cout;
    using std::endl;

    #define PRINTORRETURN(a) cout << a << " <" << yytext << ">" << endl; return 1;
%}

WS      [ \t\v]+
NL      [\n]+

NUMBER  [0-9]
NUMBER1 [1-9]
NUMBER0 [0]

VAR     [A-Za-z_][A-Za-z0-9_]*

%%

{NUMBER1}{NUMBER}*|{NUMBER0} { PRINTORRETURN("NUMBER") }
{WS}    /* ничего                 */
{NL}    /* для работы с ошибками  */
{VAR}   { PRINTORRETURN("VAR")     }
"="     { PRINTORRETURN("=")       }
"+"     { PRINTORRETURN("+")       }
"-"     { PRINTORRETURN("-")       }
"/"     { PRINTORRETURN("/")       }
"*"     { PRINTORRETURN("*")       }
"{"     { PRINTORRETURN("{")       }
"}"     { PRINTORRETURN("}")       }
"("     { PRINTORRETURN("(")       }
")"     { PRINTORRETURN(")")       }
"while" { PRINTORRETURN("WHILE")   }
"if"    { PRINTORRETURN("IF")      }
"else"  { PRINTORRETURN("ELSE")    }
"print" { PRINTORRETURN("PRINT")   }
"?"     { PRINTORRETURN("SCAN")    }
.       { PRINTORRETURN("UNKNOWN") }

%%