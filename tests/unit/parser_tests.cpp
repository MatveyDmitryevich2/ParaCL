#define _GNU_SOURCE 1

#include <cstring>
#include <gtest/gtest.h>
#include <sstream>
#include <stdio.h>
#include <string>

#include "ast/ast.hpp"
#include "error/error.hpp"
#include "error/error_print.hpp"
#include "parser.tab.hpp"

namespace yy
{
extern int yycolno;

} // namespace yy

extern yy::parser::semantic_type* yylval;
extern yy::parser::location_type* yylloc;
extern FILE* yyin;
extern int yylineno;

class ParserTest : public ::testing::Test
{
protected:
    struct ParseResult
    {
        bool success;
        std::optional<Diagnostic> diagnostic;

        static ParseResult Success()
        {
            return {true, std::nullopt};
        }

        static ParseResult Error(Diagnostic diag)
        {
            return {false, std::move(diag)};
        }
    };

    void SetUp() override
    {
        resetGlobalState();
    }

    void TearDown() override
    {
        resetGlobalState();
    }

    void resetGlobalState()
    {

        yylineno = 1;
        yy::yycolno = 1;

        if (yyin)
        {
            fclose(yyin);
            yyin = nullptr;
        }
    }
    ParseResult parse(const std::string& code)
    {
        resetGlobalState();

        FILE* tmp = fmemopen((void*)code.c_str(), code.size(), "r");
        if (!tmp)
        {
            Diagnostic diag;
            diag.kind = DiagnosticKind::Internal;
            diag.message = "Failed to open memory stream";
            return ParseResult::Error(std::move(diag));
        }

        FILE* old_yyin = yyin;
        yyin = tmp;

        language::AST driver;
        yy::parser parser(&driver);

        try
        {
            int result = parser.parse();

            yyin = old_yyin;
            fclose(tmp);

            if (result == 0)
            {
                return ParseResult::Success();
            }
            else
            {
                Diagnostic diag;
                diag.kind = DiagnosticKind::Internal;
                diag.message = "Parser returned non-zero without exception";
                return ParseResult::Error(std::move(diag));
            }
        }
        catch (const DiagnosticError& e)
        {
            yyin = old_yyin;
            fclose(tmp);

            return ParseResult::Error(e.diagnostic());
        }
        catch (const std::exception& e)
        {
            yyin = old_yyin;
            fclose(tmp);

            Diagnostic diag;
            diag.kind = DiagnosticKind::Internal;
            diag.message = e.what();
            return ParseResult::Error(std::move(diag));
        }
    }

    void expectSuccess(const std::string& code)
    {
        auto result = parse(code);
        EXPECT_TRUE(result.success)
            << "Expected success but got error: "
            << (result.diagnostic ? result.diagnostic->message
                                  : "unknown error");
    }

    void expectError(const std::string& code, DiagnosticKind expected_kind,
                     const std::string& expected_substr = "")
    {
        auto result = parse(code);

        EXPECT_FALSE(result.success) << "Expected error but got success";
        ASSERT_TRUE(result.diagnostic.has_value()) << "No diagnostic provided";

        EXPECT_EQ(result.diagnostic->kind, expected_kind);

        if (!expected_substr.empty())
        {
            EXPECT_NE(result.diagnostic->message.find(expected_substr),
                      std::string::npos)
                << "Expected message containing: '" << expected_substr
                << "', but got: '" << result.diagnostic->message << "'";
        }
    }

    void expectSyntaxError(const std::string& code,
                           const std::string& substr = "")
    {
        expectError(code, DiagnosticKind::Syntax, substr);
    }

    void expectLexicalError(const std::string& code,
                            const std::string& substr = "")
    {
        expectError(code, DiagnosticKind::Lexical, substr);
    }

    void expectHint(const std::string& code, const std::string& expected_hint)
    {
        auto result = parse(code);

        ASSERT_FALSE(result.success) << "Expected error but got success";
        ASSERT_TRUE(result.diagnostic.has_value()) << "No diagnostic provided";

        EXPECT_NE(result.diagnostic->line_text.find("y = 10;"),
                  std::string::npos)
            << "Line text should contain 'y = 10;', but got: '"
            << result.diagnostic->line_text << "'";

        bool found = false;
        for (const auto& hint : result.diagnostic->add_message)
        {
            if (hint.find(expected_hint) != std::string::npos)
            {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found) << "Expected hint '" << expected_hint
                           << "' not found in error messages";
    }
};

// ------------------------------------------------------------
// 1. Basic tests
// ------------------------------------------------------------

TEST_F(ParserTest, NumberLiteral)
{
    expectSuccess("print 5;");
}

TEST_F(ParserTest, VariableReference)
{
    expectSuccess("x = 5; print (x);");
}

TEST_F(ParserTest, BasicAddition)
{
    expectSuccess("print 2 + 3;");
}

TEST_F(ParserTest, BasicSubtraction)
{
    expectSuccess("print 5 - 2;");
}

TEST_F(ParserTest, BasicMultiplication)
{
    expectSuccess("print 3 * 4;");
}

TEST_F(ParserTest, BasicDivision)
{
    expectSuccess("print 10 / 2;");
}

TEST_F(ParserTest, ComplexExpression)
{
    expectSuccess("print 2 + 3 * 4;");
}

TEST_F(ParserTest, ExpressionWithParentheses)
{
    expectSuccess("print (2 + 3) * 4;");
}

TEST_F(ParserTest, NestedParentheses)
{
    expectSuccess("print (2 + 3) * (4 - 1);");
}

TEST_F(ParserTest, MultipleOperations)
{
    expectSuccess("print 1 + 2 - 3 * 4 / 2;");
}

// ------------------------------------------------------------
// 2. Opers tests
// ------------------------------------------------------------

TEST_F(ParserTest, BasicAssignment)
{
    expectSuccess("x = 5;");
}

TEST_F(ParserTest, AssignmentWithExpression)
{
    expectSuccess("a = 1 + 2;");
}

TEST_F(ParserTest, MultipleAssignments)
{
    expectSuccess("x = 5; y = 10; z = x + y;");
}

TEST_F(ParserTest, Reassignment)
{
    expectSuccess("x = 5; x = x + 1;");
}

TEST_F(ParserTest, PrintStatement)
{
    expectSuccess("print (42);");
}

TEST_F(ParserTest, PrintVariable)
{
    expectSuccess("x = 10; print x;");
}

TEST_F(ParserTest, PrintExpression)
{
    expectSuccess("print 2 + 3 * 4;");
}

// ------------------------------------------------------------
// 3. Tests (if while)
// ------------------------------------------------------------

TEST_F(ParserTest, WhileLoopBasic)
{
    expectSuccess("while (0) { x = 5; }");
}

TEST_F(ParserTest, WhileLoopWithCondition)
{
    expectSuccess("x = 5; while (x > 0) { x = x - 1; }");
}

TEST_F(ParserTest, EmptyWhileBody)
{
    expectSuccess("while (1) { }");
}

TEST_F(ParserTest, IfStatement)
{
    expectSuccess("if (1) { x = 5; }");
}

TEST_F(ParserTest, IfElseStatement)
{
    expectSuccess("if (1) { x = 5; } else { x = 10; }");
}

TEST_F(ParserTest, NestedIf)
{
    expectSuccess("if (x) { if (y) { z = 1; } }");
}

// ------------------------------------------------------------
// 4. Blocks and complex programs
// ------------------------------------------------------------

TEST_F(ParserTest, EmptyBlock)
{
    expectSuccess("{ }");
}

TEST_F(ParserTest, BlockWithMultipleStatements)
{
    expectSuccess("{ x = 1; y = 2; print (x + y); }");
}

TEST_F(ParserTest, NestedBlocks)
{
    expectSuccess("{ { x = 5; } { y = 10; } }");
}

TEST_F(ParserTest, ComplexProgram_WithLogic)
{
    expectSuccess(R"(
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
     )");
}

// ------------------------------------------------------------
// 5. Scanf
// ------------------------------------------------------------

TEST_F(ParserTest, InputOperator)
{
    expectSuccess("x = ?;");
}

TEST_F(ParserTest, InputInExpression)
{
    expectSuccess("x = ? + 5;");
}

// ------------------------------------------------------------
// 6. Fails tests
// ------------------------------------------------------------

TEST_F(ParserTest, MissingSemicolon)
{
    expectSyntaxError("x = 5");
}

TEST_F(ParserTest, UnclosedParenthesis)
{
    expectSyntaxError("print (2 + 3;");
}

TEST_F(ParserTest, UnclosedBlock)
{
    expectSyntaxError("{ x = 5;");
}

TEST_F(ParserTest, UnknownToken)
{
    expectLexicalError("x = @;", "unexpected character");
}

TEST_F(ParserTest, InvalidSyntax)
{
    expectSyntaxError("x = ;");
}

TEST_F(ParserTest, MissingWhileParenthesis)
{
    expectSyntaxError("while x <= 5 { x = x + 1; }");
}

TEST_F(ParserTest, MissingIfParenthesis)
{
    expectSyntaxError("if x == 5 { print x; }");
}

TEST_F(ParserTest, ExtraClosingBrace)
{
    expectSyntaxError("{ x = 5; }}");
}

TEST_F(ParserTest, InvalidExpression)
{
    expectSyntaxError("x = 5 + * 10;");
}

// ------------------------------------------------------------
// 7. Check hints
// ------------------------------------------------------------
TEST_F(ParserTest, MissingSemicolon_HintAboutPreviousLine)
{
    auto result = parse("x = 5\ny = 10;");

    ASSERT_FALSE(result.success);
    ASSERT_TRUE(result.diagnostic.has_value());

    EXPECT_EQ(result.diagnostic->kind, DiagnosticKind::Syntax);
    EXPECT_FALSE(result.diagnostic->message.empty());
}

TEST_F(ParserTest, Recovery_MultipleErrors)
{

    auto result = parse("x = ; y = ;");
    EXPECT_FALSE(result.success);
}
