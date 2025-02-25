#include <iostream>
#include <string>
#include "Tokenizer.h"

Tokenizer::Tokenizer() : currentLine(0),
						 currentChar(0),
						 currentToken(0)
{
}

int Tokenizer::getCurrentLine()
{
	return currentLine;
}

int Tokenizer::getCurrentChar()
{
	return currentChar;
}

int Tokenizer::getCurrentToken()
{
	return currentToken;
}

void Tokenizer::tokenize(std::string input)
{
	std::cout << "Tokenizing input: " << input << "\n";
}
