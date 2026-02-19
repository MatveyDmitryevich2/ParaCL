%language "c++"
%defines
%define api.value.type variant
%define api.parser.class {parser}
%define parse.error detailed
%locations

%param { language::AST* ast }

%code requires {
    #include <string>
    #include <memory>
    #include "ast.hpp"
    #include "node.hpp"
}

%code {
    #include <iostream>

    int yylex(yy::parser::semantic_type* yylval,
              yy::parser::location_type* yylloc,
              language::AST* ast);
}

%token WHILE IF PRINT SCANF ELSE
%token EQ NE LE GE L G AND OR NOT
%token PLUS MINUS MUL DIV
%token UMINUS

%token <int> NUMBER
%token <std::string*> VAR

%type <language::IExpression*> expr
%type <language::IStatement*> stmt
%type <language::BlockStmt*> stmt_list
%type <language::BlockStmt*> stmt_body
%type <language::BlockStmt*> opt_else

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
    ast->set_root($1);
    //std::cout << "✓ Parsing completed successfully\n";
}
;

stmt_list:
    %empty {
        $$ = ast->create_block();
    }
    | stmt_list stmt {
        $1->add_statement($2);
        $$ = $1;
    }
;

stmt_body:
    '{' stmt_list '}' {
        $$ = $2;
    }
    | stmt {
        $$ = ast->create_block();
        $$->add_statement($1);
    }
;

stmt:
    | VAR '=' expr ';' {
        std::string name = *$1;

        $$ = ast->create_assignment(name, $3);
    }
    | PRINT expr ';' {
        $$ = ast->create_print($2);
    }
    | WHILE '(' expr ')' stmt_body {
        $$ = ast->create_while($3, $5);
    }
    | IF '(' expr ')' stmt_body opt_else {
        $$ = ast->create_if($3, $5, $6);
    }
    | '{' stmt_list '}' {
        $$ = $2;
    }
;

opt_else:
    ELSE stmt_body { $$ = $2; }
    | %prec XIF %empty { $$ = nullptr; }
;

expr:
    MINUS expr %prec UMINUS {
        $$ = ast->create_unary(language::UnaryOp::Op::MINUS, $2);
    }
    | NOT expr {
        $$ = ast->create_unary(language::UnaryOp::Op::NOT, $2);
    }
    | expr OR expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::OR, $1, $3);
    }
    | expr AND expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::AND, $1, $3);
    }
    | expr EQ expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::EQ, $1, $3);
    }
    | expr NE expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::NE, $1, $3);
    }
    | expr L expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::L, $1, $3);
    }
    | expr G expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::G, $1, $3);
    }
    | expr LE expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::LE, $1, $3);
    }
    | expr GE expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::GE, $1, $3);
    }
    | expr PLUS expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::ADD, $1, $3);
    }
    | expr MINUS expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::SUB, $1, $3);
    }
    | expr MUL expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::MUL, $1, $3);
    }
    | expr DIV expr {
        $$ = ast->create_binary_op(language::BinaryOp::Op::DIV, $1, $3);
    }
    | '(' expr ')' {
        $$ = $2;
    }
    | NUMBER {
        $$ = ast->create_number($1);
    }
    | VAR {
        std::string name = *$1;
        $$ = ast->create_variable(name);
    }
    | SCANF {
        $$ = ast->create_scanf();
    }
;

%%

namespace yy {
    void parser::error(const location_type& loc, const std::string& msg) {
        std::cerr << "Parser ERROR at line " << loc.begin.line
                  << ": " << msg << std::endl;
    }
}
