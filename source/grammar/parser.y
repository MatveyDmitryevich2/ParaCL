%{
#include <iostream>
#include "driver.hpp"

extern int yylineno;
extern int yylex(void);

void yyerror(language::Driver* driver, const char* msg)
{
    (void)driver;
    std::cerr << "Parser ERROR at line " << yylineno << ": " << msg << std::endl;
}
%}

%code requires {
namespace language {
    class Driver;
}
#include "node.hpp"
}

%union {
    int number;
    std::string* string;
    language::IExpression* expr;
    language::IStatement* stmt;
}

%parse-param { language::Driver* driver }

%destructor { delete $$; } <expr>
%destructor { delete $$; } <stmt>
%destructor { delete $$; } <string>

%token <number> NUMBER
%token <string> VAR
%token WHILE IF PRINT SCANF ELSE
%token EQ NE LE GE L G AND OR NOT

%type <expr> expr
%type <stmt> stmt
%type <stmt> stmt_list
%type <stmt> opt_else

%nonassoc XIF
%nonassoc ELSE
%left OR
%left AND
%left EQ NE
%left LE GE L G
%left '+' '-'
%left '*' '/'
%right NOT

%%

program: stmt_list { }

stmt_list: %empty { $$ = nullptr; }
         | stmt_list stmt { $$ = nullptr; }

stmt: VAR '=' expr ';' {
    driver->add_assignment(*$1, $3);
    delete $1;
    $$ = nullptr;

}
    | PRINT '(' expr ')' ';' {
    driver->add_print($3);
    $$ = nullptr;
}
    | WHILE '(' expr ')' stmt {
    driver->add_while($3, $5);
    $$ = nullptr;
}
    | IF '(' expr ')' stmt opt_else {
    if ($6) driver->add_if($3, $5, $6);
    else driver->add_if($3, $5);
}
    | '{' stmt_list '}' { $$ = nullptr; }
;

opt_else: ELSE stmt { $$ = $2; }
        | %prec XIF %empty { $$ = nullptr; }
;

expr: expr OR expr   { $$ = driver->create_binary_op(language::BinaryOp::Op::OR,  $1, $3); }
    | expr AND expr  { $$ = driver->create_binary_op(language::BinaryOp::Op::AND, $1, $3); }
    | expr EQ expr   { $$ = driver->create_binary_op(language::BinaryOp::Op::EQ,  $1, $3); }
    | expr NE expr   { $$ = driver->create_binary_op(language::BinaryOp::Op::NE,  $1, $3); }
    | expr L expr    { $$ = driver->create_binary_op(language::BinaryOp::Op::L,   $1, $3); }
    | expr G expr    { $$ = driver->create_binary_op(language::BinaryOp::Op::G,   $1, $3); }
    | expr LE expr   { $$ = driver->create_binary_op(language::BinaryOp::Op::LE,  $1, $3); }
    | expr GE expr   { $$ = driver->create_binary_op(language::BinaryOp::Op::GE,  $1, $3); }
    | expr '+' expr  { $$ = driver->create_binary_op(language::BinaryOp::Op::ADD, $1, $3); }
    | expr '-' expr  { $$ = driver->create_binary_op(language::BinaryOp::Op::SUB, $1, $3); }
    | expr '*' expr  { $$ = driver->create_binary_op(language::BinaryOp::Op::MUL, $1, $3); }
    | expr '/' expr  { $$ = driver->create_binary_op(language::BinaryOp::Op::DIV, $1, $3); }
    | '(' expr ')'   { $$ = $2; }
    | NUMBER         { $$ = driver->create_number($1); }
    | VAR            { $$ = driver->create_variable($1); delete $1; }
    | SCANF          { $$ = driver->create_scanf(); }
;

%%
