#ifndef DBENGINE_H

#define DBENGINE_H
#include "Tokenizer.h"
#include "vector"
#include "Parser.h"
#include "string"
#include "Statement.h"
#include "Context.h"

void execute(std::vector<Statement *> statements, Context *context);
void executeCreateDatabaseStatement(std::string dbName);

#endif // DBENGINE_H