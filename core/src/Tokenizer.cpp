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
#include "Helper.h"
#include "Parser.h"

using std::cout;

std::unordered_map<std::string, TokenType> keywordMap = {
	{"create", TokenType::KEYWORD},
	{"table", TokenType::KEYWORD},
	{"insert", TokenType::KEYWORD},
	{"select", TokenType::KEYWORD},
	{"delete", TokenType::KEYWORD},
	{"update", TokenType::KEYWORD},
	{"from", TokenType::KEYWORD},
	{"where", TokenType::KEYWORD},
	{"into", TokenType::KEYWORD},
	{"values", TokenType::KEYWORD},
};

std::unordered_map<char, TokenType> symbolMap = {
	{'(', TokenType::SYMBOL},
	{')', TokenType::SYMBOL},
	{',', TokenType::SYMBOL},
	{';', TokenType::SYMBOL},
};

C_TYPE Tokenizer::readChar(char &c)
{
	if (c == ' ' || c == '\t' || c == '\n')
	{
		return C_TYPE::SPACE;
	}
	else if (isalpha(c))
	{
		return C_TYPE::CHAR;
	}
	else if (isdigit(c))
	{
		return C_TYPE::NUM;
	}
	else if (symbolMap.count(c) > 0)
	{
		return C_TYPE::SYMBOL;
	}
}

void Tokenizer::tokenize(std::string &input)
{
	// while (this->position < input.size())
	// {
	// 	char currentChar = input[position];
	// 	C_TYPE t = readChar(currentChar);
	// 	// handle string
	// 	if (C_TYPE::CHAR == t)
	// 	{
	// 		currentStringValue += input[position];
	// 		this->position++;
	// 	}
	// 	// handle num could be 5 or 55 or 555
	// 	else if (C_TYPE::NUM == t)
	// 	{
	// 	}
	// 	else if (C_TYPE::SYMBOL == t)
	// 	{
	// 		if (currentChar == ';')
	// 		{
	// 			if (!currentStringValue.empty() && keywordMap.find(currentStringValue) == keywordMap.end())
	// 			{
	// 				tokens.push_back(Token(TokenType::IDENTIFIER, currentStringValue));
	// 				currentStringValue.clear();
	// 			}
	// 			currentStringValue += currentChar;
	// 			tokens.push_back(Token(TokenType::SYMBOL, currentStringValue));
	// 			currentStringValue.clear();
	// 			break;
	// 		}
	// 		currentStringValue += currentChar;
	// 		tokens.push_back(Token(TokenType::SYMBOL, currentStringValue));
	// 		currentStringValue.clear();
	// 		this->position++;
	// 	}
	// 	// handle space
	// 	// select * from sean;
	// 	else if (C_TYPE::SPACE == t)
	// 	{
	// 		if (!currentStringValue.empty())
	// 		{
	// 			tokens.push_back(Token(keywordMap[toLowerCase(currentStringValue)], currentStringValue));
	// 			this->currentStringValue.clear();
	// 		}
	// 		this->position++;
	// 	}
	// }
	// for (auto &token : tokens)
	// {
	// 	std::cout << token.toString() << std::endl;
	// }
}