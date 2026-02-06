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
%token WHILE IF PRINT SCANF
%token EQ NE LE GE L G AND OR NOT 

%destructor { delete $$; } <string>

%left OR
%left AND
%left EQ NE
%left LE GE L G
%left '+' '-'
%left '*' '/'
%right NOT 

%%

program: stmt_list { std::cout << "Parser: Correct!\n"; }
;

stmt_list: stmt      { print_rule("stmt_list: single statement");        }
    | stmt_list stmt { print_rule("stmt_list: another statement added"); }
;

stmt:
      VAR '=' expr ';'        { print_rule("stmt: assignment");      }
    | PRINT '(' expr ')' ';'  { print_rule("stmt: print statement"); }
    | WHILE '(' expr ')' stmt { print_rule("stmt: while loop");      }
    | IF '(' expr ')' stmt    { print_rule("stmt: if statement");    }
    | '{' stmt_list '}'       { print_rule("stmt: block");           }
;

expr:
      expr OR expr   { print_rule("expr: logical OR");              }
    | expr AND expr  { print_rule("expr: logical AND");             }
    | NOT expr       { print_rule("expr: logical NOT");             }

    | expr EQ expr   { print_rule("expr: ==");                      }
    | expr NE expr   { print_rule("expr: !=");                      }
    | expr L  expr   { print_rule("expr: <");                       }
    | expr G  expr   { print_rule("expr: >");                       }
    | expr LE expr   { print_rule("expr: <=");                      }
    | expr GE expr   { print_rule("expr: >=");                      }

    | expr '+' expr  { print_rule("expr: addition");                }
    | expr '-' expr  { print_rule("expr: subtraction");             }
    | expr '*' expr  { print_rule("expr: multiplication");          }
    | expr '/' expr  { print_rule("expr: division");                }

    | '(' expr ')'   { print_rule("expr: parentheses");             }

    | NUMBER         { print_rule("expr: number literal");          } 
    | VAR            { print_rule("expr: variable reference");      }

    | SCANF          { print_rule("expr: read from stdin (scanf)"); }
;

%%

