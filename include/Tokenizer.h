#ifndef Tokenizer_H
#define Tokenizer_H
#include <string>
#include <iostream>

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
	std::string toString()
	{
		return "Value: " + value + " Type: " + std::to_string(static_cast<int>(type));
	}
};

class Tokenizer
{
public:
	void tokenize(std::string &input);
};

#endif // Tokenizer_H