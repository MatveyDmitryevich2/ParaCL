%option noyywrap
%option yylineno
%option nounput
%option noinput

%{
#include "parser.tab.hpp"
#include <string>
#include <iostream>


extern yy::parser::semantic_type* yylval;
extern yy::parser::location_type* yylloc;
extern language::AST* ast_ptr;
%}

%%

[0-9]+ {
    yylval->emplace<int>(std::stoi(yytext));
    return yy::parser::token::NUMBER;
}

"while"   { return yy::parser::token::WHILE; }
"if"      { return yy::parser::token::IF;    }
"print"   { return yy::parser::token::PRINT; }
"?"       { return yy::parser::token::SCANF; }
"else"    { return yy::parser::token::ELSE;  }

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval->emplace<std::string*>(ast_ptr->make_string(yytext));
    return yy::parser::token::VAR;
}

[ \t\r\n]+ { /* for ignoring \n */ }

"+"  { return yy::parser::token::PLUS;  }
"-"  { return yy::parser::token::MINUS; }
"*"  { return yy::parser::token::MUL;   }
"/"  { return yy::parser::token::DIV;   }

"==" { return yy::parser::token::EQ;    }
"!=" { return yy::parser::token::NE;    }
"<=" { return yy::parser::token::LE;    }
">=" { return yy::parser::token::GE;    }
"<"  { return yy::parser::token::L;     }
">"  { return yy::parser::token::G;     }

"&&" { return yy::parser::token::AND;   }
"||" { return yy::parser::token::OR;    }
"!"  { return yy::parser::token::NOT;   }

"("  { return '('; }
")"  { return ')'; }
"{"  { return '{'; }
"}"  { return '}'; }
"="  { return yy::parser::token::EQUALS; }
";"  { return ';'; }

. {
    std::cerr << "Unexpected character: " << yytext[0] << std::endl;
    return yytext[0];
}

%%

yy::parser::semantic_type* yylval;
yy::parser::location_type* yylloc;
language::AST* ast_ptr = nullptr;

int yylex(yy::parser::semantic_type* yylval_param,
          yy::parser::location_type* yylloc_param,
          language::AST* ast_param) {
    yylval = yylval_param;
    yylloc = yylloc_param;
    ast_ptr = ast_param;

    yylloc->begin.line = yylineno;
    int token = yylex();
    yylloc->end.line = yylineno;

    return token;
}
