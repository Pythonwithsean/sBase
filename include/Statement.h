#ifndef STATEMENT_H

#define STATEMENT_H
#include <string>
#include <vector>

class Statement
{
public:
	virtual ~Statement() = default;

private:
};

class CreateDatabaseStatement : public Statement
{
public:
	CreateDatabaseStatement(std::string dbName) : dbName(dbName) {}
	std::string getDbName() const { return dbName; }

private:
	std::string dbName;
};

class CreateTableStatement : public Statement
{
public:
	CreateTableStatement(std::string tableName, std::vector<std::pair<std::string, std::string>> columns)
		: tableName(tableName), columns(columns) {}

private:
	std::string tableName;
	std::vector<std::pair<std::string, std::string>> columns; // Pair of column name and type
};

class useDatabaseStatement : public Statement
{
public:
	useDatabaseStatement(std::string dbName) : dbName(dbName) {}
	std::string getDbName() const { return dbName; }

private:
	std::string dbName;
};

#endif // STATEMENT_H