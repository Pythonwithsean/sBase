#include <string>
#include <iostream>
#ifndef Tokenizer_H
#define Tokenizer_H

class Tokenizer
{
private:
	int currentLine;
	int currentChar;
	int currentToken;

public:
	Tokenizer();
	int getCurrentLine() const;
	int getCurrentChar() const;
	int getCurrentToken() const;
	void tokenize(std::string &input);
};

#endif // Tokenizer_H