# Check if Google Test is installed in the system
find_package(GTest QUIET)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


include(GoogleTest)
cmake_minimum_required(VERSION 3.10)

project(SQLiteExtensionTest)

find_package(SQLite3 REQUIRED)

add_library(vector_extension SHARED vector_extension.cpp)
target_link_libraries(vector_extension ${SQLite3_LIBRARIES})

add_executable(test_extension test_extension.cpp)
target_link_libraries(test_extension GTest::gtest GTest::gtest_main ${SQLite3_LIBRARIES})

add_executable(test_vec test_vec.cpp)
target_link_libraries(test_vec GTest::gtest GTest::gtest_main ${SQLite3_LIBRARIES})