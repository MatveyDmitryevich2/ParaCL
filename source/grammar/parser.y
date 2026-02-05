%{
#include <iostream>
#include <string>

extern int yylineno;
extern int yylex(void);

void yyerror(const char* msg)
{
    std::cerr << "Parser ERROR at line " << yylineno << ": " << msg << std::endl;
}

void print_rule(const char* rule)
{
    std::cout << "Parser: " << rule << std::endl;
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

program: stmt_list { std::cout << "Parser: Correct!\n"; }
;

stmt_list: stmt { print_rule("stmt_list: single statement"); }
    | stmt_list stmt { print_rule("stmt_list: another statement added"); }
;

stmt:
    VAR '=' expr ';' {
        print_rule("stmt: assignment");
        delete $1;
    }
    | PRINT '(' expr ')' ';' { print_rule("stmt: print statement"); }
    | WHILE '(' expr ')' stmt { print_rule("stmt: while loop"); }
    | IF '(' expr ')' stmt { print_rule("stmt: if statement"); }
    | '{' stmt_list '}' { print_rule("stmt: block"); }
;

expr:
    expr '+' expr { print_rule("expr: addition"); }
    | expr '-' expr { print_rule("expr: subtraction"); }
    | expr '*' expr { print_rule("expr: multiplication"); }
    | expr '/' expr { print_rule("expr: division"); }
    | '(' expr ')' { print_rule("expr: parentheses"); }
    | NUMBER { print_rule("expr: number literal"); }
    | VAR {
        print_rule("expr: variable reference");
        delete $1;
    }
;

%%

