%{
#include <iostream>
#include <string>

extern int yylineno;
extern int yylex(void);

void yyerror(const char* s);

void print_rule(const char* rule)
{
    std::cout << rule << std::endl;
}
%}

%code requires {
#include <string>
}

%union {
    int number;
    std::string* string;
}

%token <number> NUMBER
%token <string> VAR
%token WHILE IF PRINT

%left '+' '-'
%left '*' '/'

%%

program: stmt_list { std::cout << " Correct!\n"; }
;

stmt_list: stmt | stmt_list stmt
;

stmt:
    VAR '=' expr ';' {
        print_rule("assign");
        delete $1;
    }
    | PRINT '(' expr ')' ';' { print_rule("Print"); }
    | WHILE '(' expr ')' stmt { print_rule("While"); }
    | IF '(' expr ')' stmt { print_rule("If"); }
    | '{' stmt_list '}' { print_rule("Block"); }
;

expr:
    expr '+' expr { print_rule("ADD"); }
    | expr '-' expr { print_rule("SUB"); }
    | expr '*' expr { print_rule("MUL"); }
    | expr '/' expr { print_rule("DIV"); }
    | '(' expr ')' { print_rule("Parens"); }
    | NUMBER { print_rule("NUM"); }
    | VAR {
        print_rule("VAR");
        delete $1;
    }
;

%%

void yyerror(const char* msg)
{
    std::cerr << "ERROR " << yylineno << ": " << msg << std::endl;
}
