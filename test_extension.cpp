#include <iostream>
#include <sqlite3.h>
#include <gtest/gtest.h>

// Setup a test fixture to handle database setup and teardown.
sqlite3 *db = nullptr;
class SQLiteExtensionTest : public ::testing::Test
{
protected:
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
    const char *sql = "SELECT VECTOR_DESERIALIZE(VECTOR_ADD(VECTOR(1.0), VECTOR(2.0)));";
    sqlite3_stmt *stmt;
    ASSERT_EQ(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr), SQLITE_OK);

    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    std::string expected = "[3]";
    std::string got = (const char *)sqlite3_column_text(stmt, 0);
    ASSERT_EQ(got, expected);

    sqlite3_finalize(stmt);
}

TEST_F(SQLiteExtensionTest, VectorNN)
{
    // Create three vectors (1.0), (2.0), (3.0)
    // Query NN to (3.1)
    const char *sql = "CREATE TABLE test (vec BLOB);"
                      "INSERT INTO test VALUES (VECTOR(1.0));"
                      "INSERT INTO test VALUES (VECTOR(2.0));"
                      "INSERT INTO test VALUES (VECTOR(3.0));"
                      "SELECT VECTOR_NN(VECTOR(3.1), 'test.vec');";
    std::vector<char*> sqls;
    sqls.push_back("CREATE TABLE test (vec BLOB);");
    sqls.push_back("INSERT INTO test VALUES (VECTOR(1.0));");
    sqls.push_back("INSERT INTO test VALUES (VECTOR(2.0));");
    sqls.push_back("INSERT INTO test VALUES (VECTOR(3.0));");

    for (auto sql : sqls)
    {
        sqlite3_stmt *stmt;
        ASSERT_EQ(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr), SQLITE_OK);
        ASSERT_EQ(sqlite3_step(stmt), SQLITE_DONE);
        sqlite3_finalize(stmt);
    }

    const char *sql2 = "SELECT VECTOR_DESERIALIZE(VECTOR_NN(VECTOR(3.1), 'test.vec'));";
    sqlite3_stmt *stmt2;
    ASSERT_EQ(sqlite3_prepare_v2(db, sql2, -1, &stmt2, nullptr), SQLITE_OK);

    ASSERT_EQ(sqlite3_step(stmt2), SQLITE_ROW);
    std::string expected = "[3]";
    std::string got = (const char *)sqlite3_column_text(stmt2, 0);
    ASSERT_EQ(got, expected);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
