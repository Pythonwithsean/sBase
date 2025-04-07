#include <iostream>
#include <string>

class Parser
{
public:
	Parser(const std::string &filePath);
	void parse();
	void printParsedData() const;
};