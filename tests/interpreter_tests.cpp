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
    FILE* old_yyin = nullptr;
    int old_yylineno = 0;

    void SetUp() override
    {
        old_yyin = yyin;
        old_yylineno = yylineno;
        yyin = nullptr;
        yylineno = 1;
    }

    void TearDown() override
    {
        if (yyin != nullptr && yyin != old_yyin)
        {
            fclose(yyin);
        }
        yyin = old_yyin;
        yylineno = old_yylineno;
    }

    std::string run(const std::string& code)
    {
        FILE* tmp = fmemopen((void*)code.c_str(), code.size(), "r");
        yyin = tmp;
        yylineno = 1;

        language::AST ast;
        yy::parser parser(&ast);
        if (parser.parse() != 0)
        {
            return "";
        }

        testing::internal::CaptureStdout();

        try
        {
            language::Interpreter interp;
            interp.Run(*ast.get_root());
        }
        catch (...)
        {

            testing::internal::GetCapturedStdout();
            throw;
        }

        return testing::internal::GetCapturedStdout();
    }

    int getVarValue(const std::string& code, const std::string& var)
    {
        FILE* tmp = fmemopen((void*)code.c_str(), code.size(), "r");
        yyin = tmp;
        yylineno = 1;

        language::AST ast;
        yy::parser parser(&ast);
        parser.parse();

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

TEST_F(InterpreterTest, PriorityDivision)
{
    EXPECT_EQ(run("print 10 + 20 / 4 ;"), "15\n");
}

TEST_F(InterpreterTest, PriorityMul)
{
    EXPECT_EQ(run("print 10 + 20 * 4 ;"), "90\n");
}

TEST_F(InterpreterTest, PriorityBracket)
{
    EXPECT_EQ(run("print (10 + 20) * 4 ;"), "120\n");
}

TEST_F(InterpreterTest, UnaryMinus_Complex)
{
    EXPECT_EQ(run("print -2 * 3 ;"), "-6\n");
    EXPECT_EQ(run("print 2 * -3 ;"), "-6\n");
    EXPECT_EQ(run("print -2 * -3 ;"), "6\n");
}

TEST_F(InterpreterTest, UnaryMinus_Priority)
{
    EXPECT_EQ(run("print -2 + 2 * 2 ;"), "2\n");
    EXPECT_EQ(run("print -2 * 2 + 2 ;"), "-2\n");
}

TEST_F(InterpreterTest, UnaryMinus_Brackets)
{
    EXPECT_EQ(run("print -(2 + 3) ;"), "-5\n");
    EXPECT_EQ(run("print -(2 * 3) ;"), "-6\n");
    EXPECT_EQ(run("print -((2 + 3) * 2) ;"), "-10\n");
}

TEST_F(InterpreterTest, MultipleUnary)
{
    EXPECT_EQ(run("print - -5;"), "5\n");
    EXPECT_EQ(run("print - - -5;"), "-5\n");
    EXPECT_EQ(run("print ! !5;"), "1\n");
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

TEST_F(InterpreterTest, NestedAssignments)
{
    EXPECT_EQ(run("i = (j = (k = 5)); print i; print j; print k;"),
              "5\n5\n5\n");
    EXPECT_EQ(run("i = (j = 5) + (k = 3); print i; print j; print k;"),
              "8\n5\n3\n");
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

TEST_F(InterpreterTest, AssignInIfCondition)
{
    EXPECT_EQ(run("if (i = 1) { print i; }"), "1\n");
    EXPECT_EQ(run("if (i = 0) { print 42; } else { print i; }"), "0\n");
}

TEST_F(InterpreterTest, NestedIfElse)
{
    EXPECT_EQ(run("if (1) if (0) print 1; else print 2; else print 3;"), "2\n");
}

TEST_F(InterpreterTest, ComparisonInIf)
{
    EXPECT_EQ(run("x = 5; if (x > 3) { print 1; } else { print 0; }"), "1\n");
    EXPECT_EQ(run("x = 2; if (x >= 3) { print 1; } else { print 0; }"), "0\n");
    EXPECT_EQ(run("x = 3; if (x == 3) { print 1; }"), "1\n");
    EXPECT_EQ(run("x = 3; if (x != 3) { print 1; } else { print 0; }"), "0\n");
}

TEST_F(InterpreterTest, EmptyBlocks)
{
    EXPECT_EQ(run("if (0) { } print 1;"), "1\n");
    EXPECT_EQ(run("while (0) { } print 1;"), "1\n");
    EXPECT_EQ(run("{ } print 1;"), "1\n");
}

TEST_F(InterpreterTest, ZeroAndNegative)
{
    EXPECT_EQ(run("x = 0; if (!x) { print 1; }"), "1\n");
    EXPECT_EQ(run("x = -1; if (x) { print 1; } else { print 0; }"), "1\n");
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

TEST_F(InterpreterTest, DivisionByZero)
{
    EXPECT_THROW(run("print(5 / 0);"), std::runtime_error);
}

TEST_F(InterpreterTest, UndefinedVariable_Throws)
{
    EXPECT_THROW(run("print(unknown);"), std::runtime_error);
}
