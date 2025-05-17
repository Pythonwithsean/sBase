#include <gtest/gtest.h>
#include <vector>
#include "Tokenizer.h"
#include "Parser.h"

class ParserTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		// Setup code if needed
	}

	void TearDown() override
	{
		// Cleanup code if needed
	}
};

TEST_F(ParserTest, ValidCreateDatabase)
{
	std::string input = "CREATE DATABASE testDB;";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_NO_THROW(parser.parse());
}

TEST_F(ParserTest, ValidCreateTable)
{
	std::string input = "CREATE TABLE testTable;";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_NO_THROW(parser.parse());
}

TEST_F(ParserTest, invalidSyntax2)
{
	std::string input = "CREATE DATBASE ;";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_THROW(parser.parse(), std::runtime_error);
}

TEST_F(ParserTest, invalidSyntax3)
{
	std::string input = "CREATE DATABASE;";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_THROW(parser.parse(), std::runtime_error);
}

TEST_F(ParserTest, InvalidSyntax)
{
	std::string input = "CREATE ;";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_THROW(parser.parse(), std::runtime_error);
}

TEST_F(ParserTest, EmptyInput)
{
	std::string input = "";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_THROW(parser.parse(), std::runtime_error);
}