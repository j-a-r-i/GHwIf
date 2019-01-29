#include "db.h"
#include "logger.h"
#include <string>

//#define VERBOSE

/**
 *  @arg fname  Database name can be also ":memory:"
 */
Database::Database(const char* fname)
{
    int stat = sqlite3_open(fname, &_db);
    if (stat != SQLITE_OK) {
	Log::err("database open", sqlite3_errmsg(_db));
    }
}

Database::~Database()
{
    int stat = sqlite3_close(_db);
    if (stat != SQLITE_OK) {
	Log::err("database close", sqlite3_errmsg(_db));
    }
}

sqlite3_stmt *Database::Prepare(const char* sql)
{
    sqlite3_stmt *stmt;
    int stat = sqlite3_prepare(_db, sql, -1, &stmt, NULL);
    if (stat != SQLITE_OK) {
	Log::err("database query", sqlite3_errmsg(_db));
	Log::msg("database query", sql);
	return NULL;
    }
    return stmt;
}

void Database::version()
{
    Log::msg("version sqlite", SQLITE_VERSION);
}


Query::Query(Database *db, const char* sql)
{
    _statement = db->Prepare(sql);
}

Query::~Query()
{
}

void Query::Handle()
{
	if (_statement == NULL)
		return;

	while (sqlite3_step(_statement) == SQLITE_ROW) {
		HandleRow();
	}
	sqlite3_finalize(_statement);
}

/**
 The default implementation of HandleRow, which just dump the contents of the row.
 Normally subclasses should handle this.
*/
void Query::HandleRow()
{
#ifdef VERBOSE
    Log::msg("database row", "ROW:");
#endif
	for (int i = 0; i < sqlite3_column_count(_statement); i++) {
		std::string val;

		switch (sqlite3_column_type(_statement, i)) {
		case SQLITE_INTEGER:
			val = std::to_string(sqlite3_column_int(_statement, i));
			break;
		case SQLITE_FLOAT:
			val = std::to_string(sqlite3_column_double(_statement, i));
			break;
		case SQLITE_TEXT:
			val = (const char*)sqlite3_column_text(_statement, i);
			break;
		default:
			val = "unknown type";
			break;
		}
#ifdef VERBOSE
		Log::value(sqlite3_column_name(_statement, i), val.c_str());
#endif
	}
}
