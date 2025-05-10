#ifndef Tokenizer_H
#define Tokenizer_H
#include <vector>
#include <string>

enum class TokenType
{
	IDENTIFIER,
	KEYWORD,
	OPERATOR,
	STRING_LITERAL,
	NUMBER_LITERAL,
	COMMENT,
	SYMBOL
};

struct Token
{
	std::string tokenTypeToString(TokenType tokenType)
	{
		switch (tokenType)
		{
		case TokenType::IDENTIFIER:
			return "IDENTIFIER";
		case TokenType::KEYWORD:
			return "KEYWORD";
		case TokenType::OPERATOR:
			return "OPERATOR";
		case TokenType::STRING_LITERAL:
			return "STRING_LITERAL";
		case TokenType::NUMBER_LITERAL:
			return "NUMBER_LITERAL";
		case TokenType::COMMENT:
			return "COMMENT";
		case TokenType::SYMBOL:
			return "SYMBOL";
		default:
			return "UNKNOWN";
		}
	}
	TokenType type;
	std::string value;
	Token(TokenType t, const std::string &v) : type(t), value(v) {}
	std::string toString()
	{
		return "Token: " + tokenTypeToString(type) + ", Value: " + value;
	}
};

class Tokenizer
{
private:
	std::vector<Token> tokens;

public:
	void tokenize(std::string &input);
};

#endif // Tokenizer_H