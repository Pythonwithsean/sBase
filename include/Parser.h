#ifndef Parser_H
#define Parser_H

class Parser
{
private:
	std::vector<Token> tokens;
	long unsigned int currentTokenPointer;
	Token *currentToken;

public:
	Parser(std::vector<Token> &tokens);
	Token getCurrent() const;
	void next();
	bool canGetNext() const;
	void parse();
	void printParsedData() const;
	void parseCreateDatabase();
	void parseCreate();
	void parseCreateTable();
};

#endif // Parser_H