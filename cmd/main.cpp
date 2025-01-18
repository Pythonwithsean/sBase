#include <iostream>
#include <vector>

#define MIN_ARGS 10

int main(int argc, char *argv[])
{
	// -h localhost -p 8080 -u pythonwithsean -p idk123 -db test
	std::vector<char *> argsArr = {};
	if (argc < MIN_ARGS)
	{
		printf("[Error] Must Enter all args to connect to database! \n");
		return 1;
	}
}
