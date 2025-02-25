#include <iostream>
#include <cstring>
#include <string>
#include <Tokenizer.h>

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
	std::cout << "sBase> ";
}

int main(int argc, char *argv[])
{

	printHelp();
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
	while (true)
	{
		std::string input;
		printPrompt();
		std::getline(std::cin, input);
		// trim input
		if (input == "exit")
		{
			std::cout << "See you Next Time! :)\n";
			std::cout << "Bye Bye\n";
			break;
		}
		else if (input == ".help")
		{
			printHelp();
		}
		else if (input == ".version")
		{
			printVersion();
		}
		else
		{
			Tokenizer tokenizer;
			tokenizer.tokenize(input);
		}
	}

	return 0;
}
