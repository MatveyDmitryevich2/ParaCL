#include "parser.tab.hpp" // из ${CMAKE_CURRENT_BINARY_DIR}
#include <gtest/gtest.h>
#include <string>

extern int yyparse();
extern void yyrestart(FILE* input);

class ParserTest : public ::testing::Test
{
protected:
    bool parse(const std::string& code)
    {

        FILE* tmp = fmemopen((void*)code.c_str(), code.size(), "r");
        if (!tmp)
            return false;

        yyrestart(tmp);
        int result = yyparse();
        fclose(tmp);

        return result == 0;
    }
};

// ------------------------------------------------------------
// 1. Basic tests
// ------------------------------------------------------------

TEST_F(ParserTest, NumberLiteral)
{
    EXPECT_TRUE(parse("print (5);"));
}

TEST_F(ParserTest, VariableReference)
{
    EXPECT_TRUE(parse("x = 5; print (x);"));
}

TEST_F(ParserTest, BasicAddition)
{
    EXPECT_TRUE(parse("print (2 + 3);"));
}

TEST_F(ParserTest, BasicSubtraction)
{
    EXPECT_TRUE(parse("print (5 - 2);"));
}

TEST_F(ParserTest, BasicMultiplication)
{
    EXPECT_TRUE(parse("print (3 * 4);"));
}

TEST_F(ParserTest, BasicDivision)
{
    EXPECT_TRUE(parse("print (10 / 2);"));
}

TEST_F(ParserTest, ComplexExpression)
{
    EXPECT_TRUE(parse("print (2 + 3 * 4);"));
}

TEST_F(ParserTest, ExpressionWithParentheses)
{
    EXPECT_TRUE(parse("print ((2 + 3) * 4);"));
}

TEST_F(ParserTest, NestedParentheses)
{
    EXPECT_TRUE(parse("print ((2 + 3) * (4 - 1));"));
}

TEST_F(ParserTest, MultipleOperations)
{
    EXPECT_TRUE(parse("print (1 + 2 - 3 * 4 / 2);"));
}

// ------------------------------------------------------------
// 2. Opers tests
// ------------------------------------------------------------

TEST_F(ParserTest, BasicAssignment)
{
    EXPECT_TRUE(parse("x = 5;"));
}

TEST_F(ParserTest, AssignmentWithExpression)
{
    EXPECT_TRUE(parse("a = 1 + 2;"));
}

TEST_F(ParserTest, MultipleAssignments)
{
    EXPECT_TRUE(parse("x = 5; y = 10; z = x + y;"));
}

TEST_F(ParserTest, Reassignment)
{
    EXPECT_TRUE(parse("x = 5; x = x + 1;"));
}

TEST_F(ParserTest, PrintStatement)
{
    EXPECT_TRUE(parse("print (42);"));
}

TEST_F(ParserTest, PrintVariable)
{
    EXPECT_TRUE(parse("x = 10; print (x);"));
}

TEST_F(ParserTest, PrintExpression)
{
    EXPECT_TRUE(parse("print (2 + 3 * 4);"));
}

// ------------------------------------------------------------
// 3. Tests (if while)
// ------------------------------------------------------------

TEST_F(ParserTest, WhileLoopBasic)
{
    EXPECT_TRUE(parse("while (0) { x = 5; }"));
}

TEST_F(ParserTest, WhileLoopWithCondition)
{
    EXPECT_TRUE(parse("x = 5; while (x > 0) { x = x - 1; }"));
}

TEST_F(ParserTest, EmptyWhileBody)
{
    EXPECT_TRUE(parse("while (1) { }"));
}

TEST_F(ParserTest, IfStatement)
{
    EXPECT_TRUE(parse("if (1) { x = 5; }"));
}

TEST_F(ParserTest, NestedIf)
{
    EXPECT_TRUE(parse("if (x) { if (y) { z = 1; } }"));
}

// ------------------------------------------------------------
// 4. Blocks and complex programs
// ------------------------------------------------------------

TEST_F(ParserTest, EmptyBlock)
{
    EXPECT_TRUE(parse("{ }"));
}

TEST_F(ParserTest, BlockWithMultipleStatements)
{
    EXPECT_TRUE(parse("{ x = 1; y = 2; print (x + y); }"));
}

TEST_F(ParserTest, NestedBlocks)
{
    EXPECT_TRUE(parse("{ { x = 5; } { y = 10; } }"));
}

TEST_F(ParserTest, ComplexProgram_WithLogic)
{
    EXPECT_TRUE(parse(R"(
        i = 0;
        sum = 0;
        while (i < 10 && !(i == 7)) {
            sum = sum + i * 2;
            if (sum >= 20 || i == 5) {
                print (sum);
            }
            i = i + 1;
        }
        print (i);
        print (sum);
    )"));
}
// ------------------------------------------------------------
// 5. Scanf
// ------------------------------------------------------------

TEST_F(ParserTest, InputOperator)
{
    EXPECT_TRUE(parse("x = ?;"));
}

TEST_F(ParserTest, InputInExpression)
{
    EXPECT_TRUE(parse("x = ? + 5;"));
}

// ------------------------------------------------------------
// 6. Fails tests
// ------------------------------------------------------------

TEST_F(ParserTest, MissingSemicolon)
{
    EXPECT_FALSE(parse("x = 5")); // Нет ;
}

TEST_F(ParserTest, UnclosedParenthesis)
{
    EXPECT_FALSE(parse("print (2 + 3;")); // Нет )
}

TEST_F(ParserTest, UnclosedBlock)
{
    EXPECT_FALSE(parse("{ x = 5;")); // Нет }
}

TEST_F(ParserTest, UnknownToken)
{
    EXPECT_FALSE(parse("x = @;")); // Неизвестный символ @
}

TEST_F(ParserTest, InvalidSyntax)
{
    EXPECT_FALSE(parse("x = ;")); // Нет выражения после =
}
