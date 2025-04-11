#ifndef Parser_H
#define Parser_H

class Parser
{
private:
	std::vector<Token> tokens;

public:
	Parser(std::vector<Token> &tokens);
	void parse();
	void printParsedData() const;
};

#endif // Parser_H