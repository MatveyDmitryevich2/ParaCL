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

TEST_F(ParserTest, BasicAssignment)
{
    EXPECT_TRUE(parse("x = 5;"));
}

TEST_F(ParserTest, PrintStatement)
{
    EXPECT_TRUE(parse("print(10);"));
}

TEST_F(ParserTest, BasicExpr)
{
    EXPECT_TRUE(parse("a = 1 + 2;"));
}
