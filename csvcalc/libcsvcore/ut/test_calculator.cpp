#include "calculator.h"

#include "CppUTest/TestHarness.h"

TEST_GROUP(calculator) {
    csv::lookup_table<int> empty_lut;
    csv::lookup_table<int> testing_lut;

    void teardown() {
        testing_lut.clear();
    }
};

TEST(calculator, test_wrong_unary_op) {
    csv::cell<int> a1("2");
    csv::cell<int> a2("=a1 + / 1");
   
    testing_lut[csv::ref::extract("a1")] = a1;
    testing_lut[csv::ref::extract("a2")] = a2;

    CHECK_THROWS(csv::syntax_error, csv::calc::expression_calc<int>(a2, testing_lut)());
}

TEST(calculator, test_wrong_op) {
    csv::cell<int> a1("2");
    csv::cell<int> a2("=a1 % 1");
   
    testing_lut[csv::ref::extract("a1")] = a1;
    testing_lut[csv::ref::extract("a2")] = a2;

    CHECK_THROWS(csv::unsupported_op, csv::calc::expression_calc<int>(a2, testing_lut)());
}

TEST(calculator, test_wrong_cell_address) {
    csv::cell<int> a1("2");
    csv::cell<int> a2("=a11 + 1");
   
    testing_lut[csv::ref::extract("a1")] = a1;
    testing_lut[csv::ref::extract("a2")] = a2;

    CHECK_THROWS(csv::bad_cell_address, csv::calc::expression_calc<int>(a2, testing_lut)());
}

TEST(calculator, test_non_number_cell_02) {
    csv::cell<int> a1("2c");
    csv::cell<int> a2("=a1 + 1");

    testing_lut[csv::ref::extract("a1")] = a1;
    testing_lut[csv::ref::extract("a2")] = a2;

    CHECK_THROWS(csv::syntax_error, csv::calc::expression_calc<int>(a2, testing_lut)());
}

TEST(calculator, test_non_number_cell_01) {
    csv::cell<int> a1("E");
    csv::cell<int> a2("=a1 + 1");

    testing_lut[csv::ref::extract("a1")] = a1;
    testing_lut[csv::ref::extract("a2")] = a2;

    CHECK_THROWS(csv::syntax_error, csv::calc::expression_calc<int>(a2, testing_lut)());
}

TEST(calculator, test_cyclic_references_big) {
    csv::cell<int> a1("=a2+a3");
    csv::cell<int> a2("=a3+b1");
    csv::cell<int> a3("=b1+b2");
    csv::cell<int> b1("=b2+b3");
    csv::cell<int> b2("=b3+c1");
    csv::cell<int> b3("=c1+c2");
    csv::cell<int> c1("=c2+c3");
    csv::cell<int> c2("=c3+1");
    csv::cell<int> c3("=a1");

    testing_lut[csv::ref::extract("a1")] = a1;
    testing_lut[csv::ref::extract("a2")] = a2;
    testing_lut[csv::ref::extract("a3")] = a3;
    testing_lut[csv::ref::extract("b1")] = b1;
    testing_lut[csv::ref::extract("b2")] = b2;
    testing_lut[csv::ref::extract("b3")] = b3;
    testing_lut[csv::ref::extract("c1")] = c1;
    testing_lut[csv::ref::extract("c2")] = c2;
    testing_lut[csv::ref::extract("c3")] = c3;

    CHECK_THROWS(csv::cyclic_reference, csv::calc::expression_calc<int>(a1, testing_lut)());
}

TEST(calculator, test_cyclic_references_small) {
    csv::cell<int> a1("=a2+1");
    csv::cell<int> a2("=a1+2");

    testing_lut[csv::ref::extract("a1")] = a1;
    testing_lut[csv::ref::extract("a2")] = a2;

    CHECK_THROWS(csv::cyclic_reference, csv::calc::expression_calc<int>(a1, testing_lut)());
    CHECK_THROWS(csv::cyclic_reference, csv::calc::expression_calc<int>(a2, testing_lut)());
}

TEST(calculator, test_cyclic_references_tiny) {
    csv::cell<int> a1("=a1+1");

    testing_lut[csv::ref::extract("a1")] = a1;

    CHECK_THROWS(csv::cyclic_reference, csv::calc::expression_calc<int>(a1, testing_lut)());
}

TEST(calculator, test_reference_chaining_reversed) {
    csv::cell<int> a1("=a2+a3");
    csv::cell<int> a2("=a3+b1");
    csv::cell<int> a3("=b1+b2");
    csv::cell<int> b1("=b2+b3");
    csv::cell<int> b2("=b3+c1");
    csv::cell<int> b3("=c1+c2");
    csv::cell<int> c1("=c2+c3");
    csv::cell<int> c2("=c3+1");
    csv::cell<int> c3("1");

    testing_lut[csv::ref::extract("a1")] = a1;
    testing_lut[csv::ref::extract("a2")] = a2;
    testing_lut[csv::ref::extract("a3")] = a3;
    testing_lut[csv::ref::extract("b1")] = b1;
    testing_lut[csv::ref::extract("b2")] = b2;
    testing_lut[csv::ref::extract("b3")] = b3;
    testing_lut[csv::ref::extract("c1")] = c1;
    testing_lut[csv::ref::extract("c2")] = c2;
    testing_lut[csv::ref::extract("c3")] = c3;

    CHECK_EQUAL(1, csv::calc::expression_calc<int>(c3, testing_lut)());
    CHECK_EQUAL(2, csv::calc::expression_calc<int>(c2, testing_lut)());
    CHECK_EQUAL(3, csv::calc::expression_calc<int>(c1, testing_lut)());
    CHECK_EQUAL(5, csv::calc::expression_calc<int>(b3, testing_lut)());
    CHECK_EQUAL(8, csv::calc::expression_calc<int>(b2, testing_lut)());
    CHECK_EQUAL(13, csv::calc::expression_calc<int>(b1, testing_lut)());
    CHECK_EQUAL(21, csv::calc::expression_calc<int>(a3, testing_lut)());
    CHECK_EQUAL(34, csv::calc::expression_calc<int>(a2, testing_lut)());
    CHECK_EQUAL(55, csv::calc::expression_calc<int>(a1, testing_lut)());
}

TEST(calculator, test_reference_chaining) {
    csv::cell<int> a1("=a2+a3");
    csv::cell<int> a2("=a3+b1");
    csv::cell<int> a3("=b1+b2");
    csv::cell<int> b1("=b2+b3");
    csv::cell<int> b2("=b3+c1");
    csv::cell<int> b3("=c1+c2");
    csv::cell<int> c1("=c2+c3");
    csv::cell<int> c2("=c3+1");
    csv::cell<int> c3("1");

    testing_lut[csv::ref::extract("a1")] = a1;
    testing_lut[csv::ref::extract("a2")] = a2;
    testing_lut[csv::ref::extract("a3")] = a3;
    testing_lut[csv::ref::extract("b1")] = b1;
    testing_lut[csv::ref::extract("b2")] = b2;
    testing_lut[csv::ref::extract("b3")] = b3;
    testing_lut[csv::ref::extract("c1")] = c1;
    testing_lut[csv::ref::extract("c2")] = c2;
    testing_lut[csv::ref::extract("c3")] = c3;

    CHECK_EQUAL(55, csv::calc::expression_calc<int>(a1, testing_lut)());
    CHECK_EQUAL(34, csv::calc::expression_calc<int>(a2, testing_lut)());
    CHECK_EQUAL(21, csv::calc::expression_calc<int>(a3, testing_lut)());
    CHECK_EQUAL(13, csv::calc::expression_calc<int>(b1, testing_lut)());
    CHECK_EQUAL(8, csv::calc::expression_calc<int>(b2, testing_lut)());
    CHECK_EQUAL(5, csv::calc::expression_calc<int>(b3, testing_lut)());
    CHECK_EQUAL(3, csv::calc::expression_calc<int>(c1, testing_lut)());
    CHECK_EQUAL(2, csv::calc::expression_calc<int>(c2, testing_lut)());
    CHECK_EQUAL(1, csv::calc::expression_calc<int>(c3, testing_lut)());
}

TEST(calculator, test_references_simple) {
    csv::cell<int> a1("1");
    csv::cell<int> bc10("=A1+1");
    csv::cell<int> def100("=100+BC10");

    testing_lut[csv::ref::extract("A1")] = a1;
    testing_lut[csv::ref::extract("BC10")] = bc10;
    testing_lut[csv::ref::extract("DEF100")] = def100;

    CHECK_EQUAL(1, csv::calc::expression_calc<int>(a1, testing_lut)());
    CHECK_EQUAL(2, csv::calc::expression_calc<int>(bc10, testing_lut)());
    CHECK_EQUAL(102, csv::calc::expression_calc<int>(def100, testing_lut)());
}

TEST(calculator, test_expr_with_leading_spaces) {
    csv::cell<int> cell("   =1");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL(1, res);
}

TEST(calculator, test_const_with_spaces) {
    csv::cell<int> cell("= 1 ");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL(1, res);
}

TEST(calculator, test_const_expr_division_by_zero) {
    csv::cell<int> cell("=4 / (2 * 2 - 8 / 2) - 2 * (-5)");
    CHECK_THROWS(csv::division_by_zero, csv::calc::expression_calc(cell, empty_lut)());
}

TEST(calculator, test_const_expr_09) {
    csv::cell<int> cell("=(((-1)*-2)*-3)*-4 + 5");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL((((-1)*-2)*-3)*-4 + 5, res);
}

TEST(calculator, test_const_expr_08) {
    csv::cell<int> cell("=1 + (1 + (1 + (1 + (+1))))");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL(1 + (1 + (1 + (1 + (+1)))), res);
}

TEST(calculator, test_const_expr_07) {
    csv::cell<int> cell("=4 / (2 * 2 - 10 / 2) - 2 * (-5)");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL(4 / (2 * 2 - 10 / 2) - 2 * (-5), res);
}

TEST(calculator, test_const_expr_06) {
    csv::cell<int> cell("=4 / (2 * 2) - 2 * -5");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL(4 / (2 * 2) - 2 * -5, res);
}

TEST(calculator, test_const_expr_05) {
    csv::cell<int> cell("=(2 + 2) * 2");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL((2 + 2) * 2, res);
}

TEST(calculator, test_const_expr_04) {
    csv::cell<int> cell("=2 + 2 * 2");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL(2 + 2 * 2, res);
}

TEST(calculator, test_const_expr_03) {
    csv::cell<int> cell("=+10");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL(10, res);
}

TEST(calculator, test_const_expr_02) {
    csv::cell<int> cell("=-10");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL(-10, res);
}

TEST(calculator, test_const_expr_01) {
    csv::cell<int> cell("=10");
    int res = csv::calc::expression_calc(cell, empty_lut)();
    CHECK_EQUAL(10, res);
}

TEST(calculator, test_divide_by_zero) {
    int a = 714;
    int b = 0;
    CHECK_THROWS(csv::division_by_zero, csv::calc::divide(a, b));
}

TEST(calculator, test_divide) {
    int a = 714;
    int b = 110;
    CHECK_EQUAL(a / b, csv::calc::divide(a, b));
}

TEST(calculator, test_multiply) {
    int a = 123;
    int b = 321;
    CHECK_EQUAL(a * b, csv::calc::multiply(a, b));
}

TEST(calculator, test_minus) {
    int a = 10;
    int b = 100;
    CHECK_EQUAL(a - b, csv::calc::minus(a, b));
}

TEST(calculator, test_plus) {
    int a = 10;
    int b = 100;
    CHECK_EQUAL(a + b, csv::calc::plus(a, b));
}

TEST(calculator, test_unary_minus) {
    int a = 5;
    CHECK_EQUAL(-a, csv::calc::unary_minus(a));
}

TEST(calculator, test_unary_plus) {
    int a = 5;
    CHECK_EQUAL(a, csv::calc::unary_plus(a));
}

TEST(calculator, test_conv_double) {
    std::string s{"12.3"};
    double v = csv::calc::from_string<double>(s);
    CHECK_EQUAL(12.3, v);
}

TEST(calculator, test_conv_int) {
    std::string s{"123"};
    int v = csv::calc::from_string<int>(s);
    CHECK_EQUAL(123, v);
}
