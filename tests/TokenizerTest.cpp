#include <gtest/gtest.h>
#include <vector>
#include "Tokenizer.h"

using namespace std;

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

TEST_F(TokenizerTest, ValidCreateTable1)
{
	std::string input = "CREATE TABLE SEAN(id INT,name STRING,amount INT);";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	EXPECT_EQ(tokens.size(), 14);
	EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
	EXPECT_EQ(tokens[1].type, TokenType::KEYWORD);
	EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
	EXPECT_EQ(tokens[3].type, TokenType::SYMBOL);
	EXPECT_EQ(tokens[4].type, TokenType::IDENTIFIER);
	EXPECT_EQ(tokens[5].type, TokenType::KEYWORD);
	EXPECT_EQ(tokens[6].type, TokenType::SYMBOL);
	EXPECT_EQ(tokens[7].type, TokenType::IDENTIFIER);
	EXPECT_EQ(tokens[8].type, TokenType::KEYWORD);
	EXPECT_EQ(tokens[9].type, TokenType::SYMBOL);
	EXPECT_EQ(tokens[10].type, TokenType::IDENTIFIER);
	EXPECT_EQ(tokens[11].type, TokenType::KEYWORD);
	EXPECT_EQ(tokens[12].type, TokenType::SYMBOL);
	EXPECT_EQ(tokens[13].type, TokenType::SYMBOL);
}

TEST_F(TokenizerTest, ValidCreateTable2)
{
	string input = "CREATE TABLE SEAN(id UUID,name STRING, age INT, hobbies STRING[]);";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	vector<Token> tokens = tokenizer.getTokens();
	EXPECT_EQ(tokens.size(), 17);
}

TEST_F(TokenizerTest, validNumberofTokens)
{
	std::string input = "CREATE          TABLE             SEAN             (id INT);";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	EXPECT_EQ(tokens.size(), 8);
	EXPECT_EQ(tokens[4].type, TokenType::IDENTIFIER);
	EXPECT_EQ(tokens[5].type, TokenType::KEYWORD);
}