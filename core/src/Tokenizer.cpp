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
#include <vector>
#include <algorithm>
#include <unordered_map>
#include "Tokenizer.h"
#include "Parser.h"

using std::cout;

std::unordered_map<std::string, TokenType> key_word_mapper = {
	{"CREATE", TokenType::KEYWORD},
	{"TABLE", TokenType::KEYWORD},
	{"INSERT", TokenType::KEYWORD},
	{"SELECT", TokenType::KEYWORD},
	{"DELETE", TokenType::KEYWORD},
	{"UPDATE", TokenType::KEYWORD},
	{"FROM", TokenType::KEYWORD},
	{"WHERE", TokenType::KEYWORD},
	{"INTO", TokenType::KEYWORD},
	{"VALUES", TokenType::KEYWORD},
};

Token createToken(const std::string &value)
{
	if (key_word_mapper.find(value) != key_word_mapper.end())
	{
		return Token(TokenType::KEYWORD, value);
	}
	else
	{
		return Token(TokenType::IDENTIFIER, value);
	}
}

void Tokenizer::tokenize(std::string &input)
{
	if (input.empty())
	{
		return;
	}
	std::vector<Token> tokens;
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;
	while ((pos = input.find(delimiter)) != std::string::npos)
	{
		token = input.substr(0, pos);
		tokens.push_back(createToken(token));
		input.erase(0, pos + 1);
	}
	if (input[input.length() - 1] == ';')
	{
		tokens.push_back(createToken(input.substr(0, input.length() - 1)));
		tokens.push_back(Token(TokenType::SYMBOL, ";"));
		Parser parser(tokens);
		parser.parse();
	}
	else
	{
		cout << "Invalid Syntax please add ; at the end of each statement" << "\n";
	}
}