%option noyywrap
%option yylineno
%option nounput
%option noinput

%{
#include "parser.tab.hpp"

#include <string>
#include <utility>

#include "error/error.hpp"

extern yy::parser::semantic_type* yylval;
extern yy::parser::location_type* yylloc;
extern language::AST* ast_ptr;

namespace yy {
    int yycolno = 1;
}

#define SET_LOC_BEGIN \
    yylloc->begin.line = yylineno; \
    yylloc->begin.column = yy::yycolno

#define SET_LOC_END \
    yylloc->end.line = yylineno; \
    yylloc->end.column = yy::yycolno + yyleng - 1

#define UPDATE_COLUMN \
    yy::yycolno += yyleng
%}

%%

[0-9]+ {
    SET_LOC_BEGIN;
    UPDATE_COLUMN;
    SET_LOC_END;

    try
    {
        yylval->emplace<int>(std::stoi(yytext));
    }
    catch (const std::out_of_range&)
    {
        Diagnostic diagnostic;
        diagnostic.kind = DiagnosticKind::Lexical;
        diagnostic.message = "integer literal is out of range";
        diagnostic.range = SourceRange{
            { yylineno, yy::yycolno - yyleng },
            { yylineno, yy::yycolno }
        };
        diagnostic.add_message.push_back("use a smaller integer value");

        throw LexicalError(std::move(diagnostic));
    }

    return yy::parser::token::NUMBER;
}

"while"   { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::WHILE; }
"if"      { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::IF;    }
"print"   { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::PRINT; }
"?"       { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::SCANF; }
"else"    { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::ELSE;  }

[a-zA-Z_][a-zA-Z0-9_]* {
    SET_LOC_BEGIN;
    UPDATE_COLUMN;
    SET_LOC_END;

    yylval->emplace<std::string*>(ast_ptr->make_string(yytext));
    return yy::parser::token::VAR;
}

[ \t]+ {
    SET_LOC_BEGIN;
    UPDATE_COLUMN;
    SET_LOC_END;
}

"\r\n" {
    SET_LOC_BEGIN;
    SET_LOC_END;
    yy::yycolno = 1;
}

"\n" {
    SET_LOC_BEGIN;
    SET_LOC_END;
    yy::yycolno = 1;
}

"\r" {
    SET_LOC_BEGIN;
    SET_LOC_END;
    yy::yycolno = 1;
}

"//"[^\n\r]* {
    SET_LOC_BEGIN;
    UPDATE_COLUMN;
    SET_LOC_END;
}

"==" { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::EQ;     }
"!=" { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::NE;     }
"<=" { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::LE;     }
">=" { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::GE;     }
"&&" { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::AND;    }
"||" { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::OR;     }

"+"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::PLUS;   }
"-"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::MINUS;  }
"*"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::MUL;    }
"/"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::DIV;    }
"%"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::MOD;    }
"<"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::L;      }
">"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::G;      }
"!"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::NOT;    }
"="  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return yy::parser::token::EQUALS; }

"("  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return '('; }
")"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return ')'; }
"{"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return '{'; }
"}"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return '}'; }
";"  { SET_LOC_BEGIN; UPDATE_COLUMN; SET_LOC_END; return ';'; }

. {
    SET_LOC_BEGIN;
    UPDATE_COLUMN;
    SET_LOC_END;

    Diagnostic diagnostic;
    diagnostic.kind = DiagnosticKind::Lexical;
    diagnostic.message = std::string("unexpected character '") + yytext[0] + "'";
    diagnostic.range = SourceRange{
        { yylineno, yy::yycolno - 1 },
        { yylineno, yy::yycolno }
    };
    diagnostic.add_message.push_back("remove this character or replace it with a valid token");

    throw LexicalError(std::move(diagnostic));
}

%%

yy::parser::semantic_type* yylval;
yy::parser::location_type* yylloc;
language::AST* ast_ptr = nullptr;

int yylex(yy::parser::semantic_type* yylval_param,
          yy::parser::location_type* yylloc_param,
          language::AST* ast_param)
{
    yylval = yylval_param;
    yylloc = yylloc_param;
    ast_ptr = ast_param;

    yylloc->begin.line = yylineno;
    yylloc->begin.column = yy::yycolno;

    return yylex();
}
