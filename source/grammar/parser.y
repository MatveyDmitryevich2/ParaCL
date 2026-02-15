%{
#include <iostream>
#include "ast.hpp"

extern int yylineno;
extern int yylex(void);

void yyerror(language::AST* ast, const char* msg)
{
    (void)ast;
    std::cerr << "Parser ERROR at line " << yylineno << ": " << msg << std::endl;
}
%}

%code requires {
namespace language {
    class AST;
}
#include "node.hpp"
}

%union {
    int number;
    std::string* string;
    language::IExpression* expr;
    language::IStatement* stmt;
    language::BlockStmt* block;
}

%parse-param { language::AST* ast }

%destructor { delete $$; } <expr>
%destructor { delete $$; } <stmt>
%destructor { delete $$; } <block>
%destructor { delete $$; } <string>

%token <number> NUMBER
%token <string> VAR
%token WHILE IF PRINT SCANF ELSE
%token EQ NE LE GE L G AND OR NOT
%token PLUS MINUS MUL DIV
%token UMINUS

%type <expr> expr
%type <stmt> stmt
%type <block> stmt_list
%type <stmt> opt_else

%nonassoc XIF
%nonassoc ELSE
%right NOT UMINUS
%left OR
%left AND
%left EQ NE
%left LE GE L G
%left PLUS MINUS
%left MUL DIV


%%

program: stmt_list {
    ast->set_root(std::unique_ptr<language::BlockStmt>($1));
    std::cout << "✓ Parsing completed successfully\n";
}
;

stmt_list:
    %empty {
        $$ = ast->create_block().release();
    }
    | stmt_list stmt {
        ast->add_statement_to_block($1, std::unique_ptr<language::IStatement>($2));
        $$ = $1;
    }
;

stmt: VAR '=' expr ';' {
    std::string name = std::move(*$1);
    delete $1;
    $$ = ast->create_assignment(std::move(name), $3).release();
}
    | PRINT '(' expr ')' ';' {
    $$ = ast->create_print($3).release();
}
    | WHILE '(' expr ')' stmt {

    language::BlockStmt* body_block = dynamic_cast<language::BlockStmt*>($5);
    if (!body_block) {
        body_block = ast->create_block().release();
        ast->add_statement_to_block(body_block, std::unique_ptr<language::IStatement>($5));
    }
    $$ = ast->create_while($3, body_block).release();
}
    | IF '(' expr ')' stmt opt_else {

    language::BlockStmt* then_block = dynamic_cast<language::BlockStmt*>($5);
    if (!then_block) {
        then_block = ast->create_block().release();
        ast->add_statement_to_block(then_block, std::unique_ptr<language::IStatement>($5));
    }

    language::BlockStmt* else_block = nullptr;
    if ($6) {
        else_block = dynamic_cast<language::BlockStmt*>($6);
        if (!else_block) {
            else_block = ast->create_block().release();
            ast->add_statement_to_block(else_block, std::unique_ptr<language::IStatement>($6));
        }
    }
    $$ = ast->create_if($3, then_block, else_block).release();
}
    | '{' stmt_list '}' { $$ = $2; }
;

opt_else: ELSE stmt { $$ = $2; }
        | %prec XIF %empty { $$ = nullptr; }
;

expr: MINUS expr %prec UMINUS { $$ = ast->create_unary(language::UnaryOp::Op::MINUS, $2); }
    | NOT expr                { $$ = ast->create_unary(language::UnaryOp::Op::NOT, $2); }
    | expr OR expr   { $$ = ast->create_binary_op(language::BinaryOp::Op::OR,  $1, $3); }
    | expr AND expr  { $$ = ast->create_binary_op(language::BinaryOp::Op::AND, $1, $3); }
    | expr EQ expr   { $$ = ast->create_binary_op(language::BinaryOp::Op::EQ,  $1, $3); }
    | expr NE expr   { $$ = ast->create_binary_op(language::BinaryOp::Op::NE,  $1, $3); }
    | expr L expr    { $$ = ast->create_binary_op(language::BinaryOp::Op::L,   $1, $3); }
    | expr G expr    { $$ = ast->create_binary_op(language::BinaryOp::Op::G,   $1, $3); }
    | expr LE expr   { $$ = ast->create_binary_op(language::BinaryOp::Op::LE,  $1, $3); }
    | expr GE expr   { $$ = ast->create_binary_op(language::BinaryOp::Op::GE,  $1, $3); }
    | expr PLUS expr { $$ = ast->create_binary_op(language::BinaryOp::Op::ADD, $1, $3); }
    | expr MINUS expr{ $$ = ast->create_binary_op(language::BinaryOp::Op::SUB, $1, $3); }
    | expr MUL expr  { $$ = ast->create_binary_op(language::BinaryOp::Op::MUL, $1, $3); }
    | expr DIV expr  { $$ = ast->create_binary_op(language::BinaryOp::Op::DIV, $1, $3); }
    | '(' expr ')'   { $$ = $2; }
    | NUMBER         { $$ = ast->create_number($1); }
    | VAR            { $$ = ast->create_variable($1); delete $1; }
    | SCANF          { $$ = ast->create_scanf(); }
;

%%
