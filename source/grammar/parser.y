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
%token WHILE IF PRINT SCANF ELSE // FIXME 
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
%left '+' '-' // FIXME 
%left '*' '/'
%right NOT

%%

program: stmt_list {
    driver->set_root(static_cast<language::BlockStmt*>($1));
    std::cout << " Parsing completed successfully\n";
}
;

stmt_list: 
    %empty { 
        $$ = new language::BlockStmt(); 
    }
    | stmt_list stmt {
        language::BlockStmt* block = static_cast<language::BlockStmt*>($1);
        block->add_statement(std::unique_ptr<language::IStatement>($2));
        $$ = block;
    }
;

stmt: VAR '=' expr ';' {
    $$ = new language::Assignment(*$1, std::unique_ptr<language::IExpression>($3));
    delete $1;
}
    | PRINT '(' expr ')' ';' {
    $$ = new language::PrintStmt(std::unique_ptr<language::IExpression>($3));
}
    | WHILE '(' expr ')' stmt {

    language::BlockStmt* body_block = dynamic_cast<language::BlockStmt*>($5);
    if (!body_block) {
        body_block = new language::BlockStmt();
        body_block->add_statement(std::unique_ptr<language::IStatement>($5));
    }
    $$ = new language::WhileStmt(
        std::unique_ptr<language::IExpression>($3),
        std::unique_ptr<language::IStatement>(body_block)
    );
}
    | IF '(' expr ')' stmt opt_else {

    language::BlockStmt* then_block = dynamic_cast<language::BlockStmt*>($5);
    if (!then_block) {
        then_block = new language::BlockStmt();
        then_block->add_statement(std::unique_ptr<language::IStatement>($5));
    }

    language::BlockStmt* else_block = nullptr;
    if ($6) {
        else_block = dynamic_cast<language::BlockStmt*>($6);
        if (!else_block) {
            else_block = new language::BlockStmt();
            else_block->add_statement(std::unique_ptr<language::IStatement>($6));
        }
    }
    $$ = new language::IfStmt(
        std::unique_ptr<language::IExpression>($3),
        std::unique_ptr<language::IStatement>(then_block),
        else_block ? std::unique_ptr<language::IStatement>(else_block) : nullptr
    );
}
    | '{' stmt_list '}' { $$ = $2; }
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
