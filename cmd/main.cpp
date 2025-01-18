#include <iostream>
#include <vector>

#define ARGS_LENGTH 10

int main(int argc, char *argv[])
{
	// -h localhost -p 8080 -u pythonwithsean -p idk123 -db test
	std::vector<char *> argsArr = {};
	if (argc != ARGS_LENGTH)
	{
		printf("[Error] Must Enter all args to connect to database! \n");
		return 1;
	}
}
