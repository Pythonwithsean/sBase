#ifndef CONTEXT_H
#define CONTEXT_H
#include <string>

class Context
{
public:
	Context() {};
	void setCurrentDatabase(const std::string &dbName);
	std::string getCurrentDatabase();

private:
	std::string currentDatabase;
};

#endif // CONTEXT_H