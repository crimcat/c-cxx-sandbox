set(CMAKE_CXX_STANDARD 17)

set(SOURCES src/ref.cpp src/util.cpp)
add_library(csvcore STATIC ${SOURCES})
target_include_directories(csvcore PRIVATE include)

set(TESTS ut/test_main.cpp ut/test_ref.cpp ut/test_tokenizer_rpn.cpp ut/test_calculator.cpp)
add_executable(ut_csvcore ${TESTS})
target_include_directories(ut_csvcore PRIVATE include)
target_link_libraries(ut_csvcore PRIVATE csvcore CppUTestExt CppUTest)
