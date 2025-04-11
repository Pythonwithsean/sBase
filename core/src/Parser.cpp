#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Helper.h"
#include "Tokenizer.h"
#include "Parser.h"

using std::cout;

Parser::Parser(std::vector<Token> &tokens)
{
	this->tokens = tokens;
}
void Parser::parse()
{
	for (auto &token : tokens)
	{
		if (token.type == TokenType::KEYWORD)
		{
			cout << "Parsing keyword: " << token.value << "\n";
		}
		else if (token.type == TokenType::IDENTIFIER)
		{
			cout << "Parsing identifier: " << token.value << "\n";
		}
		else if (token.type == TokenType::SYMBOL)
		{
			cout << "Parsing symbol: " << token.value << "\n";
		}
		else
		{
			cout << "Unknown token type\n";
		}
	}
};