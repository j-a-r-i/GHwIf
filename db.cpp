#include "db.h"
#include <iostream>

Database::Database(const char* fname)
{
	int stat = sqlite3_open(":memory:", &_db);
	if (stat != SQLITE_OK) {

	}
}

Database::~Database()
{
	int stat = sqlite3_close(_db);
	if (stat != SQLITE_OK) {
		std::cout << "Error closing sqlite database:" << stat << std::endl;
	}
}

sqlite3_stmt *Database::Prepare(const char* sql)
{
	sqlite3_stmt *stmt;
	int stat = sqlite3_prepare(_db, sql, -1, &stmt, NULL);
	if (stat != SQLITE_OK) {
		std::cout << "Error prepare sqlite database:" << stat << std::endl;
		return NULL;
	}
	return stmt;
}

Query::Query(Database *db, const char* sql)
{
	_statement = db->Prepare(sql);
}

Query::~Query()
{

}
