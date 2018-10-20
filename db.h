#ifndef __DB_H
#define __DB_H

#include "sqlite3.h"

//-----------------------------------------------------------------------------
class Database
{
public:
    Database(const char* fname);
    virtual ~Database();
   
	sqlite3_stmt *Prepare(const char* sql);
private:
    sqlite3 *_db;
};

//-----------------------------------------------------------------------------
class Query
{
public:
	Query(Database *db, const char* sql);
	virtual ~Query();

	void Handle();

protected:
	virtual void HandleRow();

private:
	sqlite3_stmt *_statement;
};

#endif
