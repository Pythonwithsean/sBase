#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Helper.h"
#include "Parser.h"

Parser::Parser(std::vector<std::string> &tokens)
{
	this->tokens = tokens;
}

void Parser::parse()
{
	// Implement parsing logic here
	if (tokens[tokens.size() - 1] != ";")
	{
		std::cerr << "Error: Missing semicolon at the end of the statement." << std::endl;
		return;
	}
	tokens.pop_back();
	std::string firstCommand = toLowerCase(tokens[0]);
	// Create a map of commands to their corresponding functions

	if (firstCommand == "create")
	{
	}
	else if (firstCommand == "insert")
	{
	}
	else if (firstCommand == "select")
	{
	}
	else if (firstCommand == "update")
	{
	}
	else if (firstCommand == "delete")
	{
	}
	else
	{
		std::cerr << "Error: Unknown command '" << firstCommand << "'." << std::endl;
		return;
	}
};