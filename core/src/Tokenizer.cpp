/*
 * TokenType Breakdown for SQL-like Tokenizer
 *
 * Example SQL Statement:
 *   CREATE TABLE students (id INT, name TEXT);
 *
 * Tokenized Output:
 *
 * ┌────────────┬────────────────┐
 * │   Token    │   TokenType    │
 * ├────────────┼────────────────┤
 * │ CREATE     │ KEYWORD        │
 * │ TABLE      │ KEYWORD        │
 * │ students   │ IDENTIFIER     │
 * │ (          │ SYMBOL         │
 * │ id         │ IDENTIFIER     │
 * │ INT        │ KEYWORD        │
 * │ ,          │ SYMBOL         │
 * │ name       │ IDENTIFIER     │
 * │ TEXT       │ KEYWORD        │
 * │ )          │ SYMBOL         │
 * │ ;          │ SYMBOL         │
 * └────────────┴────────────────┘
 *
 * TokenType meanings:
 * - IDENTIFIER: user-defined names (e.g. table, column, or database names)
 * - KEYWORD: reserved language words (e.g. CREATE, SELECT, DELETE)
 * - OPERATOR: logical or arithmetic operators (e.g. =, >, <, AND)
 * - STRING_LITERAL: quoted strings (e.g. 'hello')
 * - NUMBER_LITERAL: numeric values (e.g. 123, 42.5)
 * - COMMENT: comments (e.g. -- this is a comment)
 * - WHITESPACE: spaces, tabs, newlines (optional to track)
 * - SYMBOL: punctuation and grammar (e.g. (, ), ;, ,)
 */

#include <iostream>
#include <string>
#include "Tokenizer.h"
#include <vector>
#include <algorithm>
#include "Parser.h"

using std::cout;

enum class TokenType
{
	IDENTIFIER,
	KEYWORD,
	OPERATOR,
	STRING_LITERAL,
	NUMBER_LITERAL,
	COMMENT,
	WHITESPACE,
	SYMBOL
};

struct Token
{
	TokenType type;
	std::string value;
	Token(TokenType t, const std::string &v)
	{
		this->type = t;
		this->value = v;
	}
};

Tokenizer::Tokenizer()
{
}
void Tokenizer::tokenize(std::string &input)
{
	if (input.empty())
	{
		std::cout << "No input to tokenize\n";
		return;
	}
	// split the input string into tokens
	std::vector<std::string> tokens;
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;
	while ((pos = input.find(delimiter)) != std::string::npos)
	{
		token = input.substr(0, pos);
		tokens.push_back(token);
		input.erase(0, pos + 1);
	}
	if (input[input.length() - 1] == ';')
	{
		tokens.push_back(input.substr(0, input.length() - 1));
		tokens.push_back(";");
	}
	else
	{
		tokens.push_back(input);
	}
	Parser parser(tokens);
	parser.parse();
}