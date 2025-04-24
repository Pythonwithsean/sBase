#ifndef Tokenizer_H
#define Tokenizer_H
#include <vector>

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

enum class C_TYPE
{
	CHAR,
	NUM,
	SPACE
};

struct Token
{
	TokenType type;
	std::string value;
	Token(TokenType t, const std::string &v) : type(t), value(v) {}
	std::string toString()
	{
		return "Value: " + value + " Type: " + std::to_string(static_cast<int>(type));
	}
};

class Tokenizer
{
private:
	unsigned long int position;
	std::string currentStringValue;
	std::vector<Token> tokens;

public:
	Tokenizer() : position(0) {}
	void tokenize(std::string &input);
	C_TYPE readChar(char &c);
};

#endif // Tokenizer_H