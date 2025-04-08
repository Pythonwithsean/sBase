#include <iostream>
#include <string>
#include "Tokenizer.h"
#include <vector>
#include <algorithm>
#include "Parser.h"

using std::cout;

Tokenizer::Tokenizer()
{
}
void Tokenizer::tokenize(std::string &input)
{
	if (input.empty())
	{
		std::cout << "No input to tokenize\n";
		return;
	}
	// split the input string into tokens
	std::vector<std::string> tokens;
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;
	while ((pos = input.find(delimiter)) != std::string::npos)
	{
		token = input.substr(0, pos);
		tokens.push_back(token);
		input.erase(0, pos + 1);
	}
	if (input[input.length() - 1] == ';')
	{
		tokens.push_back(input.substr(0, input.length() - 1));
		tokens.push_back(";");
	}
	else
	{
		tokens.push_back(input);
	}
	Parser parser(tokens);
	parser.parse();
}