#include "csv.h"

#include <string>
#include <sstream>
#include <vector>

#include "CppUTest/TestHarness.h"

TEST_GROUP(csv_api) {
};

TEST(csv_api, test_bad_row_column) {
    std::vector<std::string> columns{"A", "B", "C"};
    std::vector<std::string> rows{"10", "20", "30"};
    std::istringstream is{", A, B, C\n10, 1, =A10 + 1, =B10 + 1\n20, =C10+1, =A20+1, =B20 + 1\n30, =2+2*2, =(2+2)*2, =0*0"};
    csv::table<int> table = csv::table<int>::extract(is);

    CHECK_THROWS(csv::bad_cell_address, table.cell_text("A", "10"));
    CHECK_THROWS(csv::bad_cell_address, table.cell_value("A", "10"));

    CHECK_THROWS(csv::bad_cell_address, table.cell_text("A", "A"));
    CHECK_THROWS(csv::bad_cell_address, table.cell_value("A", "A"));
}

TEST(csv_api, test_bad_cell_address) {
    std::vector<std::string> columns{"A", "B", "C"};
    std::vector<std::string> rows{"10", "20", "30"};
    std::istringstream is{", A, B, C\n10, 1, =A10 + 1, =B10 + 1\n20, =C10+1, =A20+1, =B20 + 1\n30, =2+2*2, =(2+2)*2, =0*0"};
    csv::table<int> table = csv::table<int>::extract(is);

    CHECK_THROWS(csv::bad_cell_address, table.cell_text("BC100"));
    CHECK_THROWS(csv::bad_cell_address, table.cell_text("100", "BC"));
    CHECK_THROWS(csv::bad_cell_address, table.cell_value("BC100"));
    CHECK_THROWS(csv::bad_cell_address, table.cell_value("100", "BC"));
}

TEST(csv_api, test_simple_csv) {
    std::vector<std::string> columns{"A", "B", "C"};
    std::vector<std::string> rows{"10", "20", "30"};
    std::istringstream is{", A, B, C\n10, 1, =A10 + 1, =B10 + 1\n20, =C10+1, =A20+1, =B20 + 1\n30, =2+2*2, =(2+2)*2, =0*0"};
    csv::table<int> table = csv::table<int>::extract(is);

    CHECK_EQUAL(3, table.columns().size());
    for(size_t i = 0; i < table.columns().size(); ++i) {
        CHECK_TRUE(columns[i] == table.columns()[i]);
    }
    CHECK_EQUAL(3, table.rows().size());
    for(size_t i = 0; i < table.rows().size(); ++i) {
        CHECK_TRUE(rows[i] == table.rows()[i]);
    }

    CHECK_TRUE(table.cell_text("A10") == " 1");
    CHECK_TRUE(table.cell_text("10", "A") == " 1");
    CHECK_EQUAL(1, table.cell_value("A10"));
    CHECK_EQUAL(1, table.cell_value("10", "A"));

    CHECK_TRUE(table.cell_text("B10") == " =A10 + 1");
    CHECK_TRUE(table.cell_text("10", "B") == " =A10 + 1");
    CHECK_EQUAL(2, table.cell_value("B10"));
    CHECK_EQUAL(2, table.cell_value("10", "B"));

    CHECK_TRUE(table.cell_text("C10") == " =B10 + 1");
    CHECK_TRUE(table.cell_text("10", "C") == " =B10 + 1");
    CHECK_EQUAL(3, table.cell_value("C10"));
    CHECK_EQUAL(3, table.cell_value("10", "C"));

    CHECK_TRUE(table.cell_text("A20") == " =C10+1");
    CHECK_TRUE(table.cell_text("20", "A") == " =C10+1");
    CHECK_EQUAL(4, table.cell_value("A20"));
    CHECK_EQUAL(4, table.cell_value("20", "A"));

    CHECK_TRUE(table.cell_text("B20") == " =A20+1");
    CHECK_TRUE(table.cell_text("20", "B") == " =A20+1");
    CHECK_EQUAL(5, table.cell_value("B20"));
    CHECK_EQUAL(5, table.cell_value("20", "B"));

    CHECK_TRUE(table.cell_text("C20") == " =B20 + 1");
    CHECK_TRUE(table.cell_text("20", "C") == " =B20 + 1");
    CHECK_EQUAL(6, table.cell_value("C20"));
    CHECK_EQUAL(6, table.cell_value("20", "C"));

    CHECK_TRUE(table.cell_text("A30") == " =2+2*2");
    CHECK_TRUE(table.cell_text("30", "A") == " =2+2*2");
    CHECK_EQUAL(6, table.cell_value("A30"));
    CHECK_EQUAL(6, table.cell_value("30", "A"));

    CHECK_TRUE(table.cell_text("B30") == " =(2+2)*2");
    CHECK_TRUE(table.cell_text("30", "B") == " =(2+2)*2");
    CHECK_EQUAL(8, table.cell_value("B30"));
    CHECK_EQUAL(8, table.cell_value("30", "B"));

    CHECK_TRUE(table.cell_text("C30") == " =0*0");
    CHECK_TRUE(table.cell_text("30", "C") == " =0*0");
    CHECK_EQUAL(0, table.cell_value("C30"));
    CHECK_EQUAL(0, table.cell_value("30", "C"));
}

TEST(csv_api, test_empty_cell_csv_with_spaces) {
    std::istringstream is{",A,B\n0,    , 1"};
    csv::table<int> table = csv::table<int>::extract(is);

    CHECK_EQUAL(0, table.cell_value("A0"));
    CHECK_EQUAL(1, table.cell_value("B0"));
}

TEST(csv_api, test_empty_cell_csv) {
    std::istringstream is{",A,B\n0,,1"};
    csv::table<int> table = csv::table<int>::extract(is);

    CHECK_EQUAL(0, table.cell_value("A0"));
    CHECK_EQUAL(1, table.cell_value("B0"));
}

TEST(csv_api, test_only_header_csv) {
    std::vector<std::string> columns{"A", "B", "C"};
    std::istringstream is{", A, B, C"};
    csv::table<int> table = csv::table<int>::extract(is);

    CHECK_EQUAL(3, table.columns().size());
    for(size_t i = 0; i < table.columns().size(); ++i) {
        CHECK_TRUE(columns[i] == table.columns()[i]);
    }
    CHECK_EQUAL(0, table.rows().size());
}

TEST(csv_api, test_empty_csv) {
    std::istringstream is{""};
    csv::table<int> table = csv::table<int>::extract(is);

    CHECK_EQUAL(0, table.columns().size());
    CHECK_EQUAL(0, table.rows().size());
}
