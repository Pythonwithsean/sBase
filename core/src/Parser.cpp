#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Helper.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "Statement.h"

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
	std::string dbName;
	if (!accept(TokenType::IDENTIFIER))
	{
		syntaxError("Expected identifier after create database");
	}
	dbName = this->getCurrent().value;
	this->next();
	if (!accept(TokenType::SYMBOL, ";"))
	{
		syntaxError("Expected symbol ';' after create database");
	}
	this->addStatement(new CreateDatabaseStatement(dbName));
	this->next();
}

void Parser::parseCreateTable()
{
	acceptCurrentToken(TokenType::IDENTIFIER);
	acceptCurrentToken(TokenType::SYMBOL, "(");
	acceptCurrentToken(TokenType::IDENTIFIER);
	acceptCurrentToken(TokenType::KEYWORD);
	while (accept(TokenType::SYMBOL, ","))
	{
		this->next();
		acceptCurrentToken(TokenType::IDENTIFIER);
		acceptCurrentToken(TokenType::KEYWORD);
	}
	acceptCurrentToken(TokenType::SYMBOL, ")");
	acceptCurrentToken(TokenType::SYMBOL, ";");
}

bool Parser::isCurrentToken() const
{
	return this->currentToken != nullptr;
}

void Parser::acceptCurrentToken(TokenType type, std::string expectedValue)
{
	if (accept(type, expectedValue))
	{
		this->next();
	}
	else
	{
		syntaxError("Expected " + expectedValue + " after " + convertTokenTypeToString(type));
	}
}

bool Parser::accept(TokenType type, std::string expectedValue)
{
	return this->isCurrentToken() && this->getCurrent().type == type && toLowerCase(this->getCurrent().value) == expectedValue;
}

bool Parser::accept(TokenType type)
{
	return this->isCurrentToken() && this->getCurrent().type == type;
}

void Parser::acceptCurrentToken(TokenType type)
{
	if (accept(type))
	{
		this->next();
	}
	else
	{
		syntaxError("Expected " + convertTokenTypeToString(type) + " but got " + this->getCurrent().value);
	}
}

void Parser::parseCreate()
{
	if (accept(TokenType::KEYWORD, "database"))
	{
		this->next();
		parseCreateDatabase();
	}
	else if (accept(TokenType::KEYWORD, "table"))
	{
		this->next();
		parseCreateTable();
	}
	else
	{
		syntaxError("Expected keyword after create");
	}
}

std::vector<Statement *> Parser::getParsedStatements() const
{
	return this->parsedStatements;
}

void Parser::addStatement(Statement *statement)
{
	this->parsedStatements.push_back(statement);
}

void Parser::parseUse()
{
	if (!accept(TokenType::IDENTIFIER))
	{
		syntaxError("Expected identifier after use");
	}
	std::string dbName = this->getCurrent().value;
	this->next();
	if (!accept(TokenType::SYMBOL, ";"))
	{
		syntaxError("Expected symbol ';' after use");
	}
	addStatement(new useDatabaseStatement(dbName));
	this->next();
}

void Parser::parse()
{

	while (this->isCurrentToken())
	{
		switch (this->getCurrent().type)
		{
		case TokenType::KEYWORD:
			if (toLowerCase(this->getCurrent().value) == "create")
			{
				acceptCurrentToken(TokenType::KEYWORD, "create");
				parseCreate();
			}
			else if (toLowerCase(this->getCurrent().value) == "use")
			{
				acceptCurrentToken(TokenType::KEYWORD, "use");
				parseUse();
			}
			else
			{
				this->next();
			}

			break;
		default:
			syntaxError("Expected keyword after start of parsing but got " + this->getCurrent().value + " index " + std::to_string(this->currentTokenPointer));
		}
	}
};
