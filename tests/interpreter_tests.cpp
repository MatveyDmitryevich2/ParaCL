#define _GNU_SOURCE 1

#include <stdio.h>
#include <string>
#include <gtest/gtest.h>

#include "ast/ast.hpp"
#include "interpreter/interpreter.hpp"
#include "parser.tab.hpp"

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

    std::string runWithInput(const std::string& code, const std::string& input)
    {

        FILE* code_file = fmemopen((void*)code.c_str(), code.size(), "r");
        FILE* saved_yyin = yyin;

        yyin = code_file;
        yylineno = 1;

        language::AST ast;
        yy::parser parser(&ast);
        if (parser.parse() != 0)
        {
            fclose(code_file);
            yyin = saved_yyin;
            return "";
        }
        fclose(code_file);

        FILE* input_file = fmemopen((void*)input.c_str(), input.size(), "r");

        testing::internal::CaptureStdout();

        try
        {
            language::Interpreter interp(input_file);
            interp.Run(*ast.get_root());
        }
        catch (...)
        {
            fclose(input_file);
            yyin = saved_yyin;
            testing::internal::GetCapturedStdout();
            throw;
        }

        std::string output = testing::internal::GetCapturedStdout();

        fclose(input_file);
        yyin = saved_yyin;

        return output;
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

TEST_F(InterpreterTest, EmptySemicolon)
{
    EXPECT_EQ(run("; print(1);"), "1\n");
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
//  Scanf tests
// ------------------------------------------------------------
TEST_F(InterpreterTest, Scanf_Basic)
{
    std::string code = "x = ?; print x;";
    std::string input = "42\n";

    EXPECT_EQ(runWithInput(code, input), "42\n");
}

TEST_F(InterpreterTest, Scanf_Multiple)
{
    std::string code = "x = ?; y = ?; print x + y;";
    std::string input = "10\n20\n";

    EXPECT_EQ(runWithInput(code, input), "30\n");
}

TEST_F(InterpreterTest, WhileScanf_StopOnZero)
{
    std::string code = R"(
        count = 0;
        while (x = ?) {
            count = count + 1;
        }
        print count;
    )";
    std::string input = "5\n3\n1\n0\n";

    EXPECT_EQ(runWithInput(code, input), "3\n");
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

// ------------------------------------------------------------
//  Comments tests
// ------------------------------------------------------------
TEST_F(InterpreterTest, Comments_SingleLine)
{
    EXPECT_EQ(run("// this is comment\nprint 5;"), "5\n");
    EXPECT_EQ(run("print 5; // inline comment"), "5\n");
}

TEST_F(InterpreterTest, Comments_MultipleLines)
{
    EXPECT_EQ(run("// line 1\n// line 2\nprint 4;"), "4\n");
}

TEST_F(InterpreterTest, Comments_InCode)
{
    EXPECT_EQ(run("x = 5; // set x\ny = x + 1; // calc y\nprint y;"), "6\n");
}

TEST_F(InterpreterTest, Comments_Empty)
{
    EXPECT_EQ(run("//\nprint 1;"), "1\n");
}
