#include <iostream>
#include <sqlite3.h>
#include <gtest/gtest.h>

// Setup a test fixture to handle database setup and teardown.
class SQLiteExtensionTest : public ::testing::Test
{
protected:
    sqlite3 *db = nullptr;

    char *errMsg = nullptr;
    void SetUp() override
    {
        ASSERT_EQ(sqlite3_open(":memory:", &db), SQLITE_OK);
        ASSERT_EQ(sqlite3_enable_load_extension(db, 1), SQLITE_OK);
        ASSERT_EQ(sqlite3_load_extension(db, "/home/leungjch/Documents/repo/sqlite-vector/build/libvector_extension.so", "sqlite3_vectorextension_init", &errMsg), SQLITE_OK);
    }

    void TearDown() override
    {
        if (db)
        {
            sqlite3_close(db);
            db = nullptr;
        }
    }
};

TEST_F(SQLiteExtensionTest, VectorConstructor)
{

    const char *sql = "SELECT VECTOR(1.0);";
    sqlite3_stmt *stmt;
    ASSERT_EQ(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr), SQLITE_OK);

    // ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    // ASSERT_EQ(sqlite3_column_int(stmt, 0), 10);

    sqlite3_finalize(stmt);
}

TEST_F(SQLiteExtensionTest, VectorAdd) 
{
    const char *sql = "SELECT ADD(VECTOR(1.0), VECTOR(2.0));";
    sqlite3_stmt *stmt;
    ASSERT_EQ(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr), SQLITE_OK);

    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    ASSERT_EQ(sqlite3_column_double(stmt, 0), 3.0);

    sqlite3_finalize(stmt);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
