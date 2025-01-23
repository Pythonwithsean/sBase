#include <iostream>
#include <cstring>
#include <string>

#define ARGS_LENGTH 11

using std::cout;

void handleInput(std::string c)
{
}

int main(int argc, char *argv[])
{
	if (argc != 2 && argc != ARGS_LENGTH)
	{
		cout << "[Error] Must Enter all args to connect to database\n"
			 << "-h <hostname> -p <port> -u <username> -p <password> -db <database>\n " << "Type -h for help \n"
			 << "\n ";
		return 1;
	}
	else if (argc == 2 && std::strcmp(argv[1], "-h") == 0)
	{
		cout << "Welcome to help please type in your complain -q to exit" << "\n";
		std::string input;
		while (true)
		{
			getline(std::cin, input);
			if (input.compare("-q") == 0)
			{
				cout << "Thanks For Using sBase : )" << "\n";
				break;
			}
			cout << input << "\n";
		}
	}

	return 0;
}
