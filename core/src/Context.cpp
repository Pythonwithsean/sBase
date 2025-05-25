#include "Context.h"

std::string Context::getCurrentDatabase()
{
	return currentDatabase;
}

void Context::setCurrentDatabase(const std::string &dbName)
{
	currentDatabase = dbName;
}
