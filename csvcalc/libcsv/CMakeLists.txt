set(CMAKE_CXX_STANDARD 17)

set(SOURCES src/csv.cpp)
add_library(csv STATIC ${SOURCES})
target_include_directories(csv PRIVATE include ../)
target_link_libraries(csv PRIVATE csvcore)

set(TESTS ut/test_main.cpp ut/test_csv_reader.cpp ut/test_csv_api.cpp)
add_executable(ut_csv ${TESTS})
target_include_directories(ut_csv PRIVATE include ../)
target_link_libraries(ut_csv PRIVATE csvcore csv CppUTestExt CppUTest)
