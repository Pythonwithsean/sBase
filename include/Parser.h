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
	void previous();
	void parse() const;
	void printParsedData() const;
	Token getPrevious() const;
	Token getNext() const;
};

#endif // Parser_H