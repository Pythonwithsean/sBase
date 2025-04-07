#include <iostream>
#include <string>
#include "Tokenizer.h"
#include <vector>
#include <algorithm>

Tokenizer::Tokenizer() : currentLine(0),
						 currentChar(0),
						 currentToken(0)
{
}

int Tokenizer::getCurrentLine() const
{
	return currentLine;
}

int Tokenizer::getCurrentChar() const
{
	return currentChar;
}

int Tokenizer::getCurrentToken() const
{
	return currentToken;
}

void Tokenizer::tokenize(std::string &input)
{

	if (input.empty())
	{
		std::cout << "No input to tokenize\n";
		return;
	}
	std::cout << input << "\n";
}