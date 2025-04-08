#ifndef Parser_H
#define Parser_H

class Parser
{
private:
	std::vector<std::string> tokens;

public:
	Parser(std::vector<std::string> &tokens);
	void parse();
	void printParsedData() const;
};

#endif // Parser_H