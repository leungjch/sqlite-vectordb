#include <sqlite3ext.h>
#include <cstddef>
#include <vector>
#include <cstring>
#include <sstream>

#include "include/vec.hpp"
#include "include/nanoflann.hpp"
#include "include/pointcloud.hpp"

SQLITE_EXTENSION_INIT1

// Convert SQLite value to Vector
Vec sqlite3_value_to_vector(sqlite3_value *value)
{
    Vec v;
    int len = sqlite3_value_bytes(value) / sizeof(double);
    v.values.resize(len);
    std::memcpy(v.values.data(), sqlite3_value_blob(value), len * sizeof(double));
    return v;
}

// Convert Vector to SQLite result
void vector_to_sqlite3_result(Vec &v, sqlite3_context *context)
{
    sqlite3_result_blob(context, v.values.data(), v.values.size() * sizeof(double), SQLITE_TRANSIENT);
}

extern "C"
{ // Using extern "C" to ensure function names are not mangled.

    void sqlite_vector_deserialize(sqlite3_context *context, int argc, sqlite3_value **argv)
    {
        // Ensure there's exactly one argument and it's a BLOB.
        if (argc != 1 || sqlite3_value_type(argv[0]) != SQLITE_BLOB)
        {
            sqlite3_result_error(context, "BLOB2TEXT: Expects a single BLOB argument.", -1);
            return;
        }

        int blobSize = sqlite3_value_bytes(argv[0]);
        const double *data = reinterpret_cast<const double *>(sqlite3_value_blob(argv[0]));

        // Number of doubles in the BLOB.
        int numDoubles = blobSize / sizeof(double);

        // Convert the BLOB data into a string representation.
        std::ostringstream oss;
        oss << "[";
        for (int i = 0; i < numDoubles; ++i)
        {
            oss << data[i];
            if (i < numDoubles - 1)
            {
                oss << ", ";
            }
        }
        oss << "]";
        std::string resultStr = oss.str();

        // Return the formatted string.
        sqlite3_result_text(context, resultStr.c_str(), -1, SQLITE_TRANSIENT);
    }

    void sqlite_vector_constructor(sqlite3_context *context, int argc, sqlite3_value **argv)
    {
        // Create a vector of size argc to hold the values.
        std::vector<double> values(argc);

        for (int i = 0; i < argc; ++i)
        {
            // Ensure the argument is of type double (REAL in SQLite).
            if (sqlite3_value_type(argv[i]) != SQLITE_FLOAT)
            {
                sqlite3_result_error(context, "VECTOR: All arguments must be of type REAL.", -1);
                return;
            }
            values[i] = sqlite3_value_double(argv[i]);
        }

        // Return the serialized vector.
        sqlite3_result_blob(context, values.data(), sizeof(double) * values.size(), SQLITE_TRANSIENT);
    }

    void sqlite_vector_add(sqlite3_context *context, int argc, sqlite3_value **argv)
    {
        Vec v1 = sqlite3_value_to_vector(argv[0]);
        printf("v1: %f\n", v1.values[0]);
        Vec v2 = sqlite3_value_to_vector(argv[1]);

        if (v1.values.size() != v2.values.size())
        {
            sqlite3_result_error(context, "Vectors are of different dimensions", -1);
            return;
        }

        for (size_t i = 0; i < v1.values.size(); ++i)
        {
            v1.values[i] += v2.values[i];
        }

        vector_to_sqlite3_result(v1, context);
    }

    // void sqlite_vector_nearest_neighbor(sqlite3_context *context, int argc, sqlite3_value **argv)
    // {
    //     if (argc != 2 || sqlite3_value_type(argv[0]) != SQLITE_BLOB || sqlite3_value_type(argv[1]) != SQLITE3_TEXT)
    //     {
    //         sqlite3_result_error(context, "VECTOR_NN: Expects a BLOB vector and a table name.", -1);
    //         return;
    //     }

    //     // Deserialize the input vector
    //     std::vector<double> queryVec;
    //     // TODO: Deserialize argv[0] into queryVec

    //     // Retrieve the table name
    //     const char *tableName = reinterpret_cast<const char *>(sqlite3_value_text(argv[1]));

    //     // TODO: Extract vectors from the specified table and populate PointCloud

    //     PointCloud cloud;
    //     // Assume the vectors in the table are 2D for simplicity; adjust as needed
    //     // cloud.pts = ...;

    //     nanoflann::KDTree index(2, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
    //     index.buildIndex();

    //     double query_pt[2] = {queryVec[0], queryVec[1]};
    //     const size_t num_results = 1;
    //     size_t ret_index;
    //     double out_dist_sqr;
    //     index.knnSearch(&query_pt[0], num_results, &ret_index, &out_dist_sqr);

    //     // Return the index of the nearest neighbor
    //     sqlite3_result_int(context, ret_index);
    // }

#ifdef _WIN32
    __declspec(dllexport)
#endif

        int sqlite3_vectorextension_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
    {
        SQLITE_EXTENSION_INIT2(pApi);
        sqlite3_create_function(db, "VECTOR", -1, SQLITE_UTF8, NULL, &sqlite_vector_constructor, NULL, NULL);
        sqlite3_create_function(db, "VECTOR_ADD", 2, SQLITE_UTF8, NULL, &sqlite_vector_add, NULL, NULL);
        sqlite3_create_function(db, "VECTOR_DESERIALIZE", 1, SQLITE_UTF8, NULL, &sqlite_vector_deserialize, NULL, NULL);
        sqlite3_create_function(
            db,                             // Database connection
            "VECTOR_NN",                    // Function name
            2,                              // Number of args
            SQLITE_UTF8,                    // Text encoding
            NULL,                           // User data pointer
            sqlite_vector_nearest_neighbor, // Scalar function implementation
            NULL,                           // Step function for aggregate (not used)
            NULL                            // Final function for aggregate (not used)
        );
        return 0;
    }

} // extern "C"