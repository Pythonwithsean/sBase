#include <iostream>
#include <string>
#include <vector>
#include "Helper.h"

std::string toLowerCase(const std::string &str)
{
	std::string result;
	for (const char c : str)
	{
		result += std::tolower(c);
	}
	return result;
}

std::string convertTokenTypeToString(TokenType type)
{
	switch (type)
	{
	case TokenType::KEYWORD:
		return "KEYWORD";
	case TokenType::IDENTIFIER:
		return "IDENTIFIER";
	case TokenType::SYMBOL:
		return "SYMBOL";
	default:
		return "UNKNOWN";
	}
}