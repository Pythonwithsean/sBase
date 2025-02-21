#include <iostream>
#include <cstring>
#include <string>

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

void parse(std::string input)
{
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <name>" << std::endl;
		return 1;
	}

	if (std::strcmp(argv[1], "-h") == 0 || std::strcmp(argv[1], "--help") == 0)
	{
		printHelp();
		return 0;
	}
	else if (std::strcmp(argv[1], "-v") == 0 || std::strcmp(argv[1], "--version") == 0)
	{
		printVersion();
		return 0;
	}

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
		std::cout << "sBase> ";
		std::getline(std::cin, input);
		std::cout << input << "\n";
	}

	return 0;
}
