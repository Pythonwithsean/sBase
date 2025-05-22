#include <gtest/gtest.h>
#include <vector>
#include "Tokenizer.h"
#include "Parser.h"

using namespace std;

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

/* Test the Create Database Command to make sure it is correct */

TEST_F(ParserTest, ValidCreateDatabase)
{
	std::string input = "CREATE DATABASE testDB;";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_NO_THROW(parser.parse());
}

TEST_F(ParserTest, ValidCreateDatabase_2)
{
	std::string input = "CREATE DATABASE testDB2;";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_NO_THROW(parser.parse());
}

TEST_F(ParserTest, ValidCreateDatabaseWithParentheses)
{
	std::string input = "CREATE DATABASE testTable; CREATE DATABASE testTable2;";
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

TEST_F(ParserTest, InvalidToken)
{
	std::string input = "CREATE DATABASE testDB; INVALID";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	std::vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_THROW(parser.parse(), std::runtime_error);
}

/*Test the create table command to make sure it is correct */

TEST_F(ParserTest, validCreateTableCommand)
{
	string input = "CREATE TABLE SEAN(id INT,name STRING,amount INT);";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_NO_THROW(parser.parse());
}

TEST_F(ParserTest, validCreateCommand2)
{
	string input = "CREATE TABLE SEAN(id UUID,name STRING, age INT, hobbies STRING[]);";
	Tokenizer tokenizer;
	tokenizer.tokenize(input);
	vector<Token> tokens = tokenizer.getTokens();
	Parser parser(tokens);
	EXPECT_NO_THROW(parser.parse());
}
