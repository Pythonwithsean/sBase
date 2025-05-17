#include <gtest/gtest.h>
#include <vector>
#include "Tokenizer.h"

class TokenizerTest : public ::testing::Test
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

TEST_F(TokenizerTest, ValidTokenize)
{
	std::string input = "CREATE DATABASE testDB;";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	EXPECT_EQ(tokens.size(), 4);
	EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
	EXPECT_EQ(tokens[1].type, TokenType::KEYWORD);
	EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
	EXPECT_EQ(tokens[3].type, TokenType::SYMBOL);
}

TEST_F(TokenizerTest, EmptyInput)
{
	std::string input = "";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	EXPECT_EQ(tokens.size(), 0);
}

TEST_F(TokenizerTest, InvalidToken)
{
	std::string input = "CREATE DATABASE testDB; INVALID";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	EXPECT_EQ(tokens.size(), 5);
	EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
	EXPECT_EQ(tokens[1].type, TokenType::KEYWORD);
	EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
	EXPECT_EQ(tokens[3].type, TokenType::SYMBOL);
	EXPECT_EQ(tokens[4].type, TokenType::IDENTIFIER);
}