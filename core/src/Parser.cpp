#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Helper.h"
#include "Tokenizer.h"
#include "Parser.h"

using namespace std;

Parser::Parser(std::vector<Token> &tokens)
{
	this->tokens = tokens;
	this->currentTokenPointer = 0;
	this->currentToken = &this->tokens[this->currentTokenPointer];
}

Token Parser::getCurrent() const
{
	return *(this->currentToken);
}
void Parser::next()
{
	if (this->currentTokenPointer + 1 < this->tokens.size())
	{
		this->currentToken = &(this->tokens[++this->currentTokenPointer]);
		return;
	}
	else
	{
		this->currentTokenPointer = this->tokens.size();
		this->currentToken = nullptr;
	}
}

bool Parser::canGetNext() const
{
	return this->currentTokenPointer < this->tokens.size();
}

void syntaxError(std::string errorMessage)
{
	throw std::runtime_error("Syntax Error: " + errorMessage);
}

void Parser::parseCreateDatabase()
{
	this->next();
	if (this->getCurrent().type == TokenType::IDENTIFIER)
	{
		this->next();
		if (this->getCurrent().type == TokenType::SYMBOL && this->getCurrent().value == ";")
		{
			this->next();
		}
		else
		{
			syntaxError("Expected symbol ';' after create database");
		}
	}
	else
	{
		syntaxError("Expected identifier after create database");
	}
}

void Parser::parseCreateTable()
{
	this->next();
	if (this->getCurrent().type == TokenType::IDENTIFIER)
	{
		this->next();
		if (this->getCurrent().type == TokenType::SYMBOL && this->getCurrent().value == ";")
		{
			this->next();
		}
		else
		{
			syntaxError("Expected symbol ';' after create table");
		}
	}
	else
	{
		syntaxError("Expected identifier after create table");
	}
}

void Parser::parseCreate()
{
	this->next();
	if (this->getCurrent().type == TokenType::KEYWORD)
	{
		if (toLowerCase(this->getCurrent().value) == "database")
		{
			parseCreateDatabase();
		}
		else if (toLowerCase(this->currentToken->value) == "table")
		{
			parseCreateTable();
		}
		else
		{
			syntaxError("Expected database or table after create");
		}
	}
	else
	{
		syntaxError("Expected keyword after create");
	}
}

void Parser::parse()
{
	if (this->tokens.empty())
	{
		syntaxError("No tokens to parse");
	}
	while (this->canGetNext())
	{
		switch (this->getCurrent().type)
		{
		case TokenType::KEYWORD:
			if (toLowerCase(this->getCurrent().value) == "create")
			{
				parseCreate();
			}
			break;
		default:
			syntaxError("Expected keyword after start of parsing but got " + this->getCurrent().value + " index " + std::to_string(this->currentTokenPointer));
		}
	}
};
