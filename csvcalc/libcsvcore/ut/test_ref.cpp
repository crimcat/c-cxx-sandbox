
#include "ref.h"
#include "exceptions.h"

#include <string>
#include <sstream>

#include "CppUTest/TestHarness.h"

TEST_GROUP(ref) {
};

TEST(ref, read_ref_ok_05_stream) {
    std::istringstream is{"ABCD0123"};
    csv::ref ref = csv::ref::extract(is);

    CHECK_EQUAL("ABCD0123", std::string(ref.to_string()));
}

TEST(ref, read_ref_ok_05) {
    csv::ref ref = csv::ref::extract("ABCD0123");

    CHECK_EQUAL("ABCD0123", std::string(ref.to_string()));
}

TEST(ref, read_ref_ok_04_stream) {
    std::istringstream is{"ABCD0123"};
    csv::ref ref = csv::ref::extract(is);

    CHECK_EQUAL("ABCD", std::string(ref.row()));
    CHECK_EQUAL("0123", std::string(ref.column()));
}

TEST(ref, read_ref_ok_04) {
    csv::ref ref = csv::ref::extract("ABCD0123");

    CHECK_EQUAL("ABCD", std::string(ref.row()));
    CHECK_EQUAL("0123", std::string(ref.column()));
}

TEST(ref, read_ref_ok_03_stream) {
    std::istringstream is{"A012"};
    csv::ref ref = csv::ref::extract(is);

    CHECK_EQUAL("A", std::string(ref.row()));
    CHECK_EQUAL("012", std::string(ref.column()));
}

TEST(ref, read_ref_ok_03) {
    csv::ref ref = csv::ref::extract("A012");

    CHECK_EQUAL("A", std::string(ref.row()));
    CHECK_EQUAL("012", std::string(ref.column()));
}

TEST(ref, read_ref_ok_02_stream) {
    std::istringstream is{"ABC0"};
    csv::ref ref = csv::ref::extract(is);

    CHECK_EQUAL("ABC", std::string(ref.row()));
    CHECK_EQUAL("0", std::string(ref.column()));
}

TEST(ref, read_ref_ok_02) {
    csv::ref ref = csv::ref::extract("ABC0");

    CHECK_EQUAL("ABC", std::string(ref.row()));
    CHECK_EQUAL("0", std::string(ref.column()));
}

TEST(ref, read_ref_ok_01_stream) {
    std::istringstream is{"A0"};
    csv::ref ref = csv::ref::extract(is);

    CHECK_EQUAL("A", std::string(ref.row()));
    CHECK_EQUAL("0", std::string(ref.column()));
}

TEST(ref, read_ref_ok_01) {
    csv::ref ref = csv::ref::extract("A0");

    CHECK_EQUAL("A", std::string(ref.row()));
    CHECK_EQUAL("0", std::string(ref.column()));
}

TEST(ref, read_ref_ok_operator) {
    std::istringstream is{"ABCD0123"};
    csv::ref ref;

    is >> ref;

    CHECK_EQUAL("ABCD", std::string(ref.row()));
    CHECK_EQUAL("0123", std::string(ref.column()));
}

TEST(ref, empty_ref_resolve) {
    csv::ref ref;

    CHECK_EQUAL("", std::string(ref.row()));
    CHECK_EQUAL("", std::string(ref.column()));
}

TEST(ref, ref_parse_exception_02) {
    std::istringstream is;
    csv::ref ref;

    is.setstate(std::ios_base::failbit);

    CHECK_THROWS(csv::bad_or_failed_input_stream, ref.extract(is));
}

TEST(ref, ref_parse_exception_01) {
    std::istringstream is;
    csv::ref ref;

    is.setstate(std::ios_base::badbit);

    CHECK_THROWS(csv::bad_or_failed_input_stream, ref.extract(is));
}

TEST(ref, bad_row_stream) {
    std::istringstream is{"123"};
    csv::ref ref;

    CHECK_THROWS(csv::bad_cell_address, csv::ref::extract(is));
}

TEST(ref, bad_row) {
    CHECK_THROWS(csv::bad_cell_address, csv::ref::extract("123"));
}

TEST(ref, bad_column_stream) {
    std::istringstream is{"ABC"};
    csv::ref ref;

    CHECK_THROWS(csv::bad_cell_address, csv::ref::extract(is));
}

TEST(ref, bad_column) {
    CHECK_THROWS(csv::bad_cell_address, csv::ref::extract("ABC"));
}
