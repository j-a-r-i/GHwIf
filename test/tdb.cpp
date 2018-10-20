#include <gtest/gtest.h>
#include "../db.h"

TEST(Db, T01)
{
    Database db("../data.db");
    Query    q(&db, "SELECT * FROM config;");

    q.Handle();
}
