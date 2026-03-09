%language "c++"
%defines
%define api.value.type variant
%define api.parser.class {parser}
%define parse.error detailed
%locations

%param { language::AST* ast }

%code requires {
    #include <string>
    #include "ast/ast.hpp"
    #include "ast/node.hpp"
    #include "error/error.hpp"
}

%code {
    #include <utility>

    namespace yy {
        extern int yycolno;
        extern std::string current_line;
        extern std::string last_complete_line;
    }

    int yylex(yy::parser::semantic_type* yylval,
              yy::parser::location_type* yylloc,
              language::AST* ast);

    SourceRange ToSourceRange(const yy::parser::location_type& loc)
    {
        return SourceRange{
            { loc.begin.line, loc.begin.column },
            { loc.end.line,   loc.end.column + 1 }
        };
    }
}

%debug

%token WHILE IF PRINT SCANF ELSE
%token EQ NE LE GE L G AND OR NOT
%token PLUS MINUS MUL DIV MOD
%token UMINUS
%token EQUALS

%token <int> NUMBER
%token <std::string*> VAR

%type <language::IExpression*> expr
%type <language::IExpression*> unary_expr
%type <language::IExpression*> primary_expr
%type <language::IStatement*> stmt
%type <language::BlockStmt*> stmt_list

%precedence XIF
%precedence ELSE
%right EQUALS
%left OR
%left AND
%left EQ NE
%left LE GE L G
%left PLUS MINUS
%left MUL DIV MOD
%right NOT UMINUS

%%

program:
    stmt_list
    {
        $1->set_range(ToSourceRange(@$));
        ast->set_root($1);
    }
;

stmt_list:
    %empty
    {
        $$ = ast->make_node<language::BlockStmt>();
        $$->set_range(ToSourceRange(@$));
    }
    | stmt_list stmt
    {
        $1->add_statement($2);
        $1->set_range(ToSourceRange(@$));
        $$ = $1;
    }
;

stmt:
    ';'
    {
        $$ = ast->make_node<language::BlockStmt>();
        $$->set_range(ToSourceRange(@$));
    }
    | VAR EQUALS expr ';'
    {
        std::string name = *$1;

        auto* assign = ast->make_node<language::Assignment>(name, $3);
        assign->set_range(ToSourceRange(@$));

        $$ = ast->make_node<language::ExpressionStmt>(assign);
        $$->set_range(ToSourceRange(@$));
    }
    | PRINT expr ';'
    {
        $$ = ast->make_node<language::PrintStmt>($2);
        $$->set_range(ToSourceRange(@$));
    }
    | WHILE '(' expr ')' stmt
    {
        $$ = ast->make_node<language::WhileStmt>($3, $5);
        $$->set_range(ToSourceRange(@$));
    }
    | IF '(' expr ')' stmt %prec XIF
    {
        $$ = ast->make_node<language::IfStmt>($3, $5, nullptr);
        $$->set_range(ToSourceRange(@$));
    }
    | IF '(' expr ')'  stmt ELSE  stmt
    {
        $$ = ast->make_node<language::IfStmt>($3, $5, $7);
        $$->set_range(ToSourceRange(@$));
    }
    | '{' stmt_list '}'
    {
        $$ = $2;
        $$->set_range(ToSourceRange(@$));
    }
;

primary_expr:
    NUMBER
    {
        $$ = ast->make_node<language::Number>($1);
        $$->set_range(ToSourceRange(@$));
    }
    | VAR
    {
        std::string name = *$1;
        $$ = ast->make_node<language::Variable>(name);
        $$->set_range(ToSourceRange(@$));
    }
    | SCANF
    {
        $$ = ast->make_node<language::ScanfExpr>();
        $$->set_range(ToSourceRange(@$));
    }
    | '(' expr ')'
    {
        $$ = $2;
    }
;

unary_expr:
    primary_expr
    | MINUS unary_expr %prec UMINUS
    {
        $$ = ast->make_node<language::UnaryOp>(language::UnaryOp::Op::UMINUS, $2);
        $$->set_range(ToSourceRange(@$));
    }
    | NOT unary_expr %prec NOT
    {
        $$ = ast->make_node<language::UnaryOp>(language::UnaryOp::Op::NOT, $2);
        $$->set_range(ToSourceRange(@$));
    }
;

expr:
    expr OR expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::OR, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr AND expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::AND, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr EQ expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::EQ, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr NE expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::NE, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr L expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::L, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr G expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::G, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr LE expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::LE, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr GE expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::GE, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr PLUS expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::ADD, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr MINUS expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::SUB, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr MUL expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::MUL, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr DIV expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::DIV, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | expr MOD expr
    {
        $$ = ast->make_node<language::BinaryOp>(language::BinaryOp::Op::MOD, $1, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | VAR EQUALS expr %prec EQUALS
    {
        std::string name = *$1;
        $$ = ast->make_node<language::Assignment>(name, $3);
        $$->set_range(ToSourceRange(@$));
    }
    | unary_expr
;

%%

void yy::parser::error(const location_type& loc, const std::string& msg)
{
    Diagnostic diagnostic;
    diagnostic.kind = DiagnosticKind::Syntax;
    diagnostic.message = msg;
    diagnostic.range = ToSourceRange(loc);

    if (!yy::current_line.empty())
    {
        diagnostic.line_text = yy::current_line;
    }
    else if (!yy::last_complete_line.empty())
    {
        diagnostic.line_text = yy::last_complete_line;
    }

    if (loc.begin.column <= 1 && !yy::last_complete_line.empty())
    {
        diagnostic.add_message.push_back("possible error near the previous line");

        if (yy::last_complete_line.back() != ';' &&
            yy::last_complete_line.back() != '{' &&
            yy::last_complete_line.back() != '}')
        {
            diagnostic.add_message.push_back("possible missing ';' at the end of the previous statement");
        }
    }

    throw ParseError(std::move(diagnostic));
}
