cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

project(csvcalc)

add_subdirectory(libcsvcore)
add_subdirectory(libcsv)

set(SOURCES src/csvcalc.cpp)

add_executable(csvcalc ${SOURCES})

target_include_directories(csvcalc PRIVATE .)
target_link_libraries(csvcalc PRIVATE csvcore csv)
