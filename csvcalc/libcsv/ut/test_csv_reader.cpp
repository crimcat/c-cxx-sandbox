#include "csv.h"

#include <string>
#include <sstream>

#include "CppUTest/TestHarness.h"

TEST_GROUP(csv_reader) {
};

TEST(csv_reader, test_one_column_csv) {
    std::istringstream is{"\t, Aa\n\t10, =1+1\n\t20, = 1 + 1 * 1\n\t30, =(2 + 2) * 2\n"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(1, d.columns.size());
    CHECK_EQUAL(3, d.rows.size());
    CHECK_EQUAL(3, d.raw_csv.size());
    CHECK_TRUE(d.columns[0] == "Aa");
    CHECK_TRUE(d.rows[0] == "10");
    CHECK_TRUE(d.rows[1] == "20");
    CHECK_TRUE(d.rows[2] == "30");
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("Aa10"));
    CHECK_TRUE(d.raw_csv.find("Aa10")->second == " =1+1");
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("Aa20"));
    CHECK_TRUE(d.raw_csv.find("Aa20")->second == " = 1 + 1 * 1");
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("Aa20"));
    CHECK_TRUE(d.raw_csv.find("Aa30")->second == " =(2 + 2) * 2");
}

TEST(csv_reader, test_simple_csv_with_spaces) {
    std::istringstream is{"\t, A , B \n 0 , 1, =2"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(2, d.columns.size());
    CHECK_EQUAL(1, d.rows.size());
    CHECK_EQUAL(2, d.raw_csv.size());
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("A0"));
    CHECK_TRUE(d.raw_csv.find("A0")->second == " 1");
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("B0"));
    CHECK_TRUE(d.raw_csv.find("B0")->second == " =2");
}

TEST(csv_reader, test_simple_csv_extra_header_trailing_spaces) {
    std::istringstream is{"          \n\n\n\n\n\n\n    ,A,B\n\n\n\n\n0,1,=2       \n\n\n\n\n\n\n\n"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(2, d.columns.size());
    CHECK_EQUAL(1, d.rows.size());
    CHECK_EQUAL(2, d.raw_csv.size());
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("A0"));
    CHECK_TRUE(d.raw_csv.find("A0")->second == "1");
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("B0"));
    CHECK_TRUE(d.raw_csv.find("B0")->second == "=2       ");
}

TEST(csv_reader, test_simple_csv) {
    std::istringstream is{",A,B\n0,1,=2"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(2, d.columns.size());
    CHECK_EQUAL(1, d.rows.size());
    CHECK_EQUAL(2, d.raw_csv.size());
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("A0"));
    CHECK_TRUE(d.raw_csv.find("A0")->second == "1");
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("B0"));
    CHECK_TRUE(d.raw_csv.find("B0")->second == "=2");
}

TEST(csv_reader, test_empty_csv_header_only_trailing_cr_random_spaces) {
    std::istringstream is{"   ,A,  B,C  ,   D   , E            \n"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(5, d.columns.size());
    CHECK_EQUAL(0, d.rows.size());
    CHECK_EQUAL(0, d.raw_csv.size());
    CHECK_TRUE(d.columns[0] == "A");
    CHECK_TRUE(d.columns[1] == "B");
    CHECK_TRUE(d.columns[2] == "C");
    CHECK_TRUE(d.columns[3] == "D");
    CHECK_TRUE(d.columns[4] == "E");
}

TEST(csv_reader, test_empty_csv_header_only_trailing_cr) {
    std::istringstream is{",A,B,C,D,E\n"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(5, d.columns.size());
    CHECK_EQUAL(0, d.rows.size());
    CHECK_EQUAL(0, d.raw_csv.size());
    CHECK_TRUE(d.columns[0] == "A");
    CHECK_TRUE(d.columns[1] == "B");
    CHECK_TRUE(d.columns[2] == "C");
    CHECK_TRUE(d.columns[3] == "D");
    CHECK_TRUE(d.columns[4] == "E");
}

TEST(csv_reader, test_empty_csv_header_only) {
    std::istringstream is{",A,B,C,D,E"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(5, d.columns.size());
    CHECK_EQUAL(0, d.rows.size());
    CHECK_EQUAL(0, d.raw_csv.size());
    CHECK_TRUE(d.columns[0] == "A");
    CHECK_TRUE(d.columns[1] == "B");
    CHECK_TRUE(d.columns[2] == "C");
    CHECK_TRUE(d.columns[3] == "D");
    CHECK_TRUE(d.columns[4] == "E");
}

TEST(csv_reader, test_simplest_csv_with_expression_03) {
    std::istringstream is{",A\n0,= 1 + 1 * 100.09 / (17 - 3)"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(1, d.columns.size());
    CHECK_EQUAL(1, d.rows.size());
    CHECK_EQUAL(1, d.raw_csv.size());
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("A0"));
    CHECK_TRUE(d.raw_csv.find("A0")->second == "= 1 + 1 * 100.09 / (17 - 3)");
}

TEST(csv_reader, test_simplest_csv_with_expression_02) {
    std::istringstream is{",A\n0,=1+1*100.09/(17-3)"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(1, d.columns.size());
    CHECK_EQUAL(1, d.rows.size());
    CHECK_EQUAL(1, d.raw_csv.size());
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("A0"));
    CHECK_TRUE(d.raw_csv.find("A0")->second == "=1+1*100.09/(17-3)");
}

TEST(csv_reader, test_simplest_csv_with_expression_01) {
    std::istringstream is{",A\n0,=1"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(1, d.columns.size());
    CHECK_EQUAL(1, d.rows.size());
    CHECK_EQUAL(1, d.raw_csv.size());
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("A0"));
    CHECK_TRUE(d.raw_csv.find("A0")->second == "=1");
}

TEST(csv_reader, test_simplest_csv_with_trailing_cr) {
    std::istringstream is{",A\n0,1\n"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(1, d.columns.size());
    CHECK_EQUAL(1, d.rows.size());
    CHECK_EQUAL(1, d.raw_csv.size());
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("A0"));
    CHECK_TRUE(d.raw_csv.find("A0")->second == "1");
}

TEST(csv_reader, test_simplest_csv) {
    std::istringstream is{",A\n0,1"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(1, d.columns.size());
    CHECK_EQUAL(1, d.rows.size());
    CHECK_EQUAL(1, d.raw_csv.size());
    CHECK_TRUE(d.raw_csv.end() != d.raw_csv.find("A0"));
    CHECK_TRUE(d.raw_csv.find("A0")->second == "1");
}

TEST(csv_reader, test_empty_cell_csv) {
    std::istringstream is{",A,B\n0,,1"};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(2, d.columns.size());
    CHECK_EQUAL(1, d.rows.size());
    CHECK_EQUAL(2, d.raw_csv.size());
}

TEST(csv_reader, test_empty) {
    std::istringstream is{""};
    csv::data d;
    csv::read(is, d);

    CHECK_EQUAL(0, d.columns.size());
    CHECK_EQUAL(0, d.rows.size());
    CHECK_EQUAL(0, d.raw_csv.size());
}
