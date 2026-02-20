#define _GNU_SOURCE 1
#include "ast.hpp"
#include "interpreter.hpp"
#include "parser.tab.hpp"
#include <gtest/gtest.h>
#include <stdio.h>
#include <string>

extern FILE* yyin;
extern int yylineno;
class InterpreterTest : public ::testing::Test
{
protected:
    std::string run(const std::string& code)
    {
        FILE* tmp = fmemopen((void*)code.c_str(), code.size(), "r");
        FILE* old_yyin = yyin;
        yyin = tmp;
        yylineno = 1;

        language::AST ast;
        yy::parser parser(&ast);
        if (parser.parse() != 0)
        {
            yyin = old_yyin;
            fclose(tmp);
            return "";
        }

        yyin = old_yyin;
        fclose(tmp);

        testing::internal::CaptureStdout();

        language::Interpreter interp;
        interp.Run(*ast.get_root());

        return testing::internal::GetCapturedStdout();
    }

    int getVarValue(const std::string& code, const std::string& var)
    {
        FILE* tmp = fmemopen((void*)code.c_str(), code.size(), "r");
        FILE* old_yyin = yyin;
        yyin = tmp;
        yylineno = 1;

        language::AST ast;
        yy::parser parser(&ast);
        parser.parse();

        yyin = old_yyin;
        fclose(tmp);

        language::Interpreter interp;
        interp.Run(*ast.get_root());

        return interp.scope_stack.GetValueVariable(var);
    }
};

// ------------------------------------------------------------
//  Arithmetic tests
// ------------------------------------------------------------
TEST_F(InterpreterTest, Addition)
{
    EXPECT_EQ(run("print 2 + 3 ;"), "5\n");
}

TEST_F(InterpreterTest, Subtraction)
{
    EXPECT_EQ(run("print 10 - 4 ;"), "6\n");
}

TEST_F(InterpreterTest, Multiplication)
{
    EXPECT_EQ(run("print 3 * 7 ;"), "21\n");
}

TEST_F(InterpreterTest, Division)
{
    EXPECT_EQ(run("print 20 / 4 ;"), "5\n");
}

// ------------------------------------------------------------
//  Variable tests
// ------------------------------------------------------------
TEST_F(InterpreterTest, Declaration)
{
    EXPECT_EQ(run("x = 5; print x;"), "5\n");
}

TEST_F(InterpreterTest, Reassignment)
{
    EXPECT_EQ(run("x = 1; x = 10; print x;"), "10\n");
}

// ------------------------------------------------------------
// Logic tests
// ------------------------------------------------------------
TEST_F(InterpreterTest, Or_False)
{
    EXPECT_EQ(run("print 0 || 0 ;"), "0\n");
}

TEST_F(InterpreterTest, Not)
{
    EXPECT_EQ(run("print !0 ;"), "1\n");
}

TEST_F(InterpreterTest, And_False)
{
    EXPECT_EQ(run("print 0 && 1 ;"), "0\n");
}

TEST_F(InterpreterTest, Or_TRUE)
{
    EXPECT_EQ(run("print 0 || 1 ;"), "1\n");
}

// ------------------------------------------------------------
//  If tests
// ------------------------------------------------------------
TEST_F(InterpreterTest, IfTrue)
{
    EXPECT_EQ(run("if (1) { print 100 ; }"), "100\n");
}

TEST_F(InterpreterTest, IfFalse)
{
    EXPECT_EQ(run("if (0) { print 100 ; }"), "");
}

// ------------------------------------------------------------
//  While tests
// ------------------------------------------------------------
TEST_F(InterpreterTest, WhileLoop)
{
    EXPECT_EQ(run("i = 0; while (i < 3) { print i ; i = i + 1; }"),
              "0\n1\n2\n");
}

// ------------------------------------------------------------
//  Error tests
// ------------------------------------------------------------
TEST_F(InterpreterTest, UndefinedVariable_Throws)
{
    language::AST ast;
    FILE* tmp = fmemopen((void*)"print unknown;", 15, "r");
    FILE* old_yyin = yyin;
    yyin = tmp;

    yy::parser parser(&ast);
    parser.parse();
    yyin = old_yyin;
    fclose(tmp);

    language::Interpreter interp;
    interp.scope_stack.AddScope();
    EXPECT_THROW(ast.get_root()->evaluate(interp), std::exception);
    interp.scope_stack.DeleteScope();
}
