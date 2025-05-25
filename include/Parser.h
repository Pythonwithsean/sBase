#ifndef Parser_H
#define Parser_H
#include <vector>
#include <Statement.h>

class Parser
{
private:
	std::vector<Token> tokens;
	long unsigned int currentTokenPointer;
	Token *currentToken;
	std::vector<Statement *> parsedStatements;

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
	void parseArgs();
	void parseArg();
	void parseUse();
	bool isCurrentToken() const;
	void acceptCurrentToken(TokenType type, std::string expectedValue);
	void acceptCurrentToken(TokenType type);
	bool accept(TokenType type, std::string expectedValue);
	bool accept(TokenType type);
	std::vector<Statement *> getParsedStatements() const;
	void addStatement(Statement *statement);
	void deleteStatements();
};

#endif // Parser_H