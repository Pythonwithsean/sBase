#include <iostream>
#include <cstring>
#include <string>
#include <Tokenizer.h>
#include <ostream>
#include <vector>
#include "Helper.h"
#include "Parser.h"
#include "Context.h"
#include "DBEngine.h"

using namespace std;

void printHelp()
{
	std::cout << "sBase - A simple Database\n"
			  << "Usage: sBase [options]\n"
			  << "Options:\n"
			  << "  -h, --help\t\tPrint this help message and exit\n"
			  << "  -v, --version\t\tPrint version information and exit\n";
}
void printVersion()
{
	std::cout << "sBase 0.1.0\n"
			  << "Written by Sean Idisi\n";
}

void printPrompt()
{
	std::cout << "sBase-> ";
}

enum class CommandType
{
	EXIT,
	HELP,
	VERSION,
	CLEAR,
	LOGO,
	UNKNOWN
};

CommandType inputToCommand(std::string input)
{
	if (input == ".exit")
		return CommandType::EXIT;
	else if (input == ".help")
		return CommandType::HELP;
	else if (input == ".version")
		return CommandType::VERSION;
	else if (input == ".clear")
		return CommandType::CLEAR;
	else if (input == ".logo")
		return CommandType::LOGO;
	return CommandType::UNKNOWN;
}

int main(int argc, char *argv[])
{
	// printHelp();
	bool RUN = true;
	Context *context = new Context();
	while (RUN)
	{
		std::string input;
		printPrompt();
		std::getline(std::cin, input);
		CommandType command = inputToCommand(toLowerCase(input));
		switch (command)
		{
		case CommandType::EXIT:
			std::cout << "See you Next Time! :)\n";
			RUN = false;
			break;
		case CommandType::HELP:
			printHelp();
			break;
		case CommandType::VERSION:
			printVersion();
			break;
		case CommandType::CLEAR:
			if (system("clear") != 0)
			{
				std::cout << "Error with the clear" << std::endl;
				return 1;
			}
			break;
		case CommandType::LOGO:
			std::cout << R"(
			______________________________________________
			|  _______  ___________  ___________  ______  |
			| /  _  \  \/  /\__  \  \/ /\__  \  \/  __  \ |
			| /   _____/\______   \_____    ______ ____  |
			| \_____  \  |    |  _/\__  \  /  ___// __ \ |
			| /        \ |    |   \ / __ \_\___ \\  ___/ |
			| /_______  / |______  /(____  /____  >\___  |
			|         \/         \/      \/     \/     \/
			|___________________________________________ |
			)"
					  << std::endl;
			break;
		case CommandType::UNKNOWN:
			Tokenizer tokenizer;
			tokenizer.tokenize(input);
			std::vector<Token> tokens = tokenizer.getTokens();
			// Parser parser(tokens);
			// vector<Statement *> parsedStatements;
			// try
			// {
			// 	if (tokens.size() > 0)
			// 	{
			// 		parser.parse();
			// 		parsedStatements = parser.getParsedStatements();
			// 		execute(parsedStatements, context);
			// 		for (auto statement : parsedStatements)
			// 		{
			// 			delete statement;
			// 		}
			// 		parsedStatements.clear();
			// 	}
			// }
			// catch (std::runtime_error &e)
			// {
			// 	cout << e.what() << endl;
			// }
		}
	}
	delete context;

	return 0;
}
