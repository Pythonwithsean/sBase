#include <iostream>
#include "DBEngine.h"
#include "filesystem"

using namespace std;

void execute(std::vector<Statement *> statements, Context *context)
{
	for (auto statement : statements)
	{
		if (dynamic_cast<CreateDatabaseStatement *>(statement))
		{
			CreateDatabaseStatement *dbStatement = dynamic_cast<CreateDatabaseStatement *>(statement);
			executeCreateDatabaseStatement(dbStatement->getDbName());
		}
		else if (dynamic_cast<useDatabaseStatement *>(statement))
		{
			useDatabaseStatement *dbStatement = dynamic_cast<useDatabaseStatement *>(statement);
			// std::cout << "Using database: " << dbStatement->getDbName() << std::endl;
			// context->setCurrentDatabase(dbStatement->getDbName());
			// std::cout << "Current database set to: " << context->getCurrentDatabase() << std::endl;
		}
		else
		{
			std::cout << "Unknown statement type." << std::endl;
		}
	}
}

void executeCreateDatabaseStatement(std::string dbName)
{
	if (dbName.empty())
	{
		std::cout << "Database name cannot be empty." << std::endl;
		return;
	}
	if (!std::filesystem::exists("./data/"))
	{
		std::filesystem::create_directory("./data/");
	}
	if (std::filesystem::exists("./data/") && std::filesystem::exists("./data/" + dbName))
	{
		std::cout << "Database already exists: " << dbName << std::endl;
		return;
	}
	else
	{
		std::filesystem::create_directory("./data/" + dbName);
	}
}
