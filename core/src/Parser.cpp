// #include <iostream>
// #include <string>
// #include <vector>
// #include <unordered_map>
// #include "Helper.h"
// #include "Tokenizer.h"
// #include "Parser.h"

// using std::cout;

// Parser::Parser(std::vector<Token> &tokens)
// {
// 	this->tokens = tokens;
// 	this->currentTokenPointer = 0;
// 	this->currentToken = &this->tokens[this->currentTokenPointer];
// }

// Token Parser::getCurrent() const
// {
// 	return *this->currentToken;
// }

// void Parser::next()
// {
// 	this->currentTokenPointer += 1;
// 	if (currentTokenPointer < tokens.size() - 1)
// 		this->currentToken = &this->tokens[this->currentTokenPointer];
// 	else
// 		this->currentTokenPointer = tokens.size() - 1;
// 	std::cout << "Current Pointer is at the end and can't go any further" << std::endl;
// }

// void Parser::previous()
// {
// 	this->currentTokenPointer -= 1;
// 	if (this->currentTokenPointer >= 0)
// 		this->currentToken = &this->tokens[this->currentTokenPointer];
// 	else
// 		this->currentTokenPointer = 0;
// 	std::cout << "Current Pointer is at the beginning and can't go any further" << std::endl;
// }

// void Parser::printParsedData() const {
// };
// void Parser::parse() const
// {
// 	// std::cout << "Parsing Tokens..." << std::endl;
// 	// std::cout << currentToken->toString() << std::endl;
// 	for (auto &token : tokens)
// 	{
// 		std::cout << "Token Value: " << token.value << " Token Type: " << std::to_string(static_cast<int>(token.type)) << std::endl;
// 	}
// };

// Token Parser::getPrevious() const
// {
// 	if (currentTokenPointer - 1 >= 0)
// 	{
// 		return (this->tokens[this->currentTokenPointer - 1]);
// 	}
// 	return this->tokens[this->currentTokenPointer];
// };

// Token Parser::getNext() const
// {
// 	if (currentTokenPointer + 1 < this->tokens.size())
// 	{
// 		return (this->tokens[this->currentTokenPointer + 1]);
// 	}
// 	return this->tokens[this->currentTokenPointer];
// };