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

using namespace std;

std::unordered_map<std::string, TokenType> keywordMap = {
	{"create", TokenType::KEYWORD},
	{"drop", TokenType::KEYWORD},
	{"alter", TokenType::KEYWORD},
	{"table", TokenType::KEYWORD},
	{"database", TokenType::KEYWORD},
	{"use", TokenType::KEYWORD},
	{"table", TokenType::KEYWORD},
	{"insert", TokenType::KEYWORD},
	{"select", TokenType::KEYWORD},
	{"delete", TokenType::KEYWORD},
	{"update", TokenType::KEYWORD},
	{"from", TokenType::KEYWORD},
	{"where", TokenType::KEYWORD},
	{"into", TokenType::KEYWORD},
	{"values", TokenType::KEYWORD},
	{"int", TokenType::KEYWORD},
	{"string", TokenType::KEYWORD},
	{"uuid", TokenType::KEYWORD},
	{"string[]", TokenType::KEYWORD},
	{"int[]", TokenType::KEYWORD},
	{"use", TokenType::KEYWORD}};

std::unordered_map<std::string, TokenType> symbolMap = {
	{"(", TokenType::SYMBOL},
	{")", TokenType::SYMBOL},
	{"*", TokenType::SYMBOL},
	{",", TokenType::SYMBOL},
	{";", TokenType::SYMBOL},
};

bool isSymbol(std::string &text)
{
	return symbolMap.count(text) > 0;
}

bool isKeyWord(std::string &text)
{
	return keywordMap.count(toLowerCase(text)) > 0;
}

TokenType getTokenType(std::string input)
{
	if (isKeyWord(input))
	{
		return TokenType::KEYWORD;
	}
	else if (isSymbol(input))
	{
		return TokenType::SYMBOL;
	}
	else
	{
		return TokenType::IDENTIFIER;
	}
}

void Tokenizer::tokenize(std::string &input)
{
	std::string currentText;
	if (input.empty())
	{
		return;
	}
	for (char &c : input)
	{
		// Escape special characters
		if (c == ' ' || c == ';' || c == '(' || c == ')' || c == ',')
		{
			if (!currentText.empty())
			{
				TokenType tokenType = getTokenType(currentText);
				tokens.push_back(Token{tokenType, currentText});
				currentText.clear();
			}
			if (c == ';')
			{
				tokens.push_back(Token{TokenType::SYMBOL, ";"});
			}
			else if (c == '(')
			{
				tokens.push_back(Token{TokenType::SYMBOL, "("});
				continue;
			}
			else if (c == ')')
			{
				tokens.push_back(Token{TokenType::SYMBOL, ")"});
				continue;
			}
			else if (c == ',')
			{
				tokens.push_back(Token{TokenType::SYMBOL, ","});
				continue;
			}
		}
		else
		{
			currentText += c;
		}
	}
	if (!currentText.empty())
	{
		tokens.push_back(Token{getTokenType(currentText), currentText});
		currentText.clear();
	}
}

std::vector<Token> Tokenizer::getTokens()
{
	return tokens;
}
