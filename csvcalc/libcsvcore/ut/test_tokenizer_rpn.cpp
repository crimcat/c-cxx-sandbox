#include "util.h"

#include <sstream>
#include <iostream>

#include "CppUTest/TestHarness.h"

TEST_GROUP(tokenizer) {
};

TEST(tokenizer, test_complex_expr_spaces) {
    std::istringstream is{"= (12.5 - A0) * B0 / ( C12 + 14 ) "};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);

    CHECK_EQUAL(13, tokens.size());

    CHECK_EQUAL(csv::util::OPEN_PAR, tokens[0].type);
    CHECK_EQUAL(csv::util::MINUS, tokens[2].type);
    CHECK_EQUAL(csv::util::CLOSE_PAR, tokens[4].type);
    CHECK_EQUAL(csv::util::MULTIPLY, tokens[5].type);
    CHECK_EQUAL(csv::util::DIVIDE, tokens[7].type);
    CHECK_EQUAL(csv::util::OPEN_PAR, tokens[8].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[10].type);
    CHECK_EQUAL(csv::util::CLOSE_PAR, tokens[12].type);

    CHECK_EQUAL(csv::util::CONST, tokens[1].type);
    CHECK_EQUAL("12.5", tokens[1].text);

    CHECK_EQUAL(csv::util::CELL_REF, tokens[3].type);
    CHECK_EQUAL("A0", tokens[3].text);

    CHECK_EQUAL(csv::util::CELL_REF, tokens[6].type);
    CHECK_EQUAL("B0", tokens[6].text);

    CHECK_EQUAL(csv::util::CELL_REF, tokens[9].type);
    CHECK_EQUAL("C12", tokens[9].text);

    CHECK_EQUAL(csv::util::CONST, tokens[11].type);
    CHECK_EQUAL("14", tokens[11].text);
}

TEST(tokenizer, test_complex_expr_no_spaces) {
    std::istringstream is{"=(12.5-A0)*B0/(C12+14)"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);

    CHECK_EQUAL(13, tokens.size());

    CHECK_EQUAL(csv::util::OPEN_PAR, tokens[0].type);
    CHECK_EQUAL(csv::util::MINUS, tokens[2].type);
    CHECK_EQUAL(csv::util::CLOSE_PAR, tokens[4].type);
    CHECK_EQUAL(csv::util::MULTIPLY, tokens[5].type);
    CHECK_EQUAL(csv::util::DIVIDE, tokens[7].type);
    CHECK_EQUAL(csv::util::OPEN_PAR, tokens[8].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[10].type);
    CHECK_EQUAL(csv::util::CLOSE_PAR, tokens[12].type);

    CHECK_EQUAL(csv::util::CONST, tokens[1].type);
    CHECK_EQUAL("12.5", tokens[1].text);

    CHECK_EQUAL(csv::util::CELL_REF, tokens[3].type);
    CHECK_EQUAL("A0", tokens[3].text);

    CHECK_EQUAL(csv::util::CELL_REF, tokens[6].type);
    CHECK_EQUAL("B0", tokens[6].text);

    CHECK_EQUAL(csv::util::CELL_REF, tokens[9].type);
    CHECK_EQUAL("C12", tokens[9].text);

    CHECK_EQUAL(csv::util::CONST, tokens[11].type);
    CHECK_EQUAL("14", tokens[11].text);
}

TEST(tokenizer, test_punct) {
    std::istringstream is{"=()+-*/"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);

    CHECK_EQUAL(6, tokens.size());
    CHECK_EQUAL(csv::util::OPEN_PAR, tokens[0].type);
    CHECK_EQUAL(csv::util::CLOSE_PAR, tokens[1].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[2].type);
    CHECK_EQUAL(csv::util::MINUS, tokens[3].type);
    CHECK_EQUAL(csv::util::MULTIPLY, tokens[4].type);
    CHECK_EQUAL(csv::util::DIVIDE, tokens[5].type);
}

TEST(tokenizer, test_expr_ref) {
    std::istringstream is{"=A0"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);

    CHECK_EQUAL(1, tokens.size());
    CHECK_EQUAL(csv::util::CELL_REF, tokens[0].type);
    CHECK_EQUAL("A0", tokens[0].text);
}

TEST(tokenizer, test_expr_const) {
    std::istringstream is{"=123.321"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);

    CHECK_EQUAL(1, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("123.321", tokens[0].text);
}

TEST(tokenizer, test_sequence) {
    std::istringstream is{"ABCabc012.012"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);

    CHECK_EQUAL(1, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("ABCabc012.012", tokens[0].text);
}

TEST(tokenizer, test_empty) {
    std::istringstream is{""};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);

    CHECK_EQUAL(0, tokens.size());
}

TEST_GROUP(tokenizer_rpn) {
};

TEST(tokenizer_rpn, test_missing_par_02) {
    std::istringstream is{"=(1+2"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);

    CHECK_THROWS(csv::syntax_error, csv::util::convert_to_rpn(tokens));
}

TEST(tokenizer_rpn, test_missing_par_01) {
    std::istringstream is{"=1+2)"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);

    CHECK_THROWS(csv::syntax_error, csv::util::convert_to_rpn(tokens));
}

TEST(tokenizer_rpn, test_par_04) {
    std::istringstream is{"=((1+1)*2)"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(5, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::CONST, tokens[1].type);
    CHECK_EQUAL("1", tokens[1].text);
    CHECK_EQUAL(csv::util::PLUS, tokens[2].type);
    CHECK_EQUAL(csv::util::CONST, tokens[3].type);
    CHECK_EQUAL("2", tokens[3].text);
    CHECK_EQUAL(csv::util::MULTIPLY, tokens[4].type);
}

TEST(tokenizer_rpn, test_par_03) {
    std::istringstream is{"=1+(1+(1+(-1)))"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(8, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::CONST, tokens[1].type);
    CHECK_EQUAL("1", tokens[1].text);
    CHECK_EQUAL(csv::util::CONST, tokens[2].type);
    CHECK_EQUAL("1", tokens[2].text);
    CHECK_EQUAL(csv::util::CONST, tokens[3].type);
    CHECK_EQUAL("1", tokens[3].text);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[4].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[5].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[6].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[7].type);
}

TEST(tokenizer_rpn, test_par_02) {
    std::istringstream is{"=(1+2)/(-A3)"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(6, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::CONST, tokens[1].type);
    CHECK_EQUAL("2", tokens[1].text);
    CHECK_EQUAL(csv::util::PLUS, tokens[2].type);
    CHECK_EQUAL(csv::util::CELL_REF, tokens[3].type);
    CHECK_EQUAL("A3", tokens[3].text);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[4].type);
    CHECK_EQUAL(csv::util::DIVIDE, tokens[5].type);
}

TEST(tokenizer_rpn, test_par_01) {
    std::istringstream is{"=(1+2)/A3"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(5, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::CONST, tokens[1].type);
    CHECK_EQUAL("2", tokens[1].text);
    CHECK_EQUAL(csv::util::PLUS, tokens[2].type);
    CHECK_EQUAL(csv::util::CELL_REF, tokens[3].type);
    CHECK_EQUAL("A3", tokens[3].text);
    CHECK_EQUAL(csv::util::DIVIDE, tokens[4].type);
}

TEST(tokenizer_rpn, test_unary_sequence_ref) {
    std::istringstream is{"=--++-+-+A200"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(9, tokens.size());
    CHECK_EQUAL(csv::util::CELL_REF, tokens[0].type);
    CHECK_EQUAL("A200", tokens[0].text);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[1].type);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[2].type);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[3].type);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[4].type);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[5].type);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[6].type);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[7].type);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[8].type);
}

TEST(tokenizer_rpn, test_unary_sequence_const) {
    std::istringstream is{"=--++-+-+1.23"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(9, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1.23", tokens[0].text);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[1].type);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[2].type);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[3].type);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[4].type);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[5].type);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[6].type);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[7].type);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[8].type);
}

TEST(tokenizer_rpn, test_simple_unary_minus_03) {
    std::istringstream is{"=-1+-1"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(5, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[1].type);
    CHECK_EQUAL(csv::util::CONST, tokens[2].type);
    CHECK_EQUAL("1", tokens[2].text);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[3].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[4].type);
}

TEST(tokenizer_rpn, test_simple_unary_plus_03) {
    std::istringstream is{"=+1++1"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(5, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[1].type);
    CHECK_EQUAL(csv::util::CONST, tokens[2].type);
    CHECK_EQUAL("1", tokens[2].text);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[3].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[4].type);
}

TEST(tokenizer_rpn, test_simple_unary_minus_02) {
    std::istringstream is{"=1+-1"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(4, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::CONST, tokens[1].type);
    CHECK_EQUAL("1", tokens[1].text);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[2].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[3].type);
}

TEST(tokenizer_rpn, test_simple_unary_plus_02) {
    std::istringstream is{"=1++1"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(4, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::CONST, tokens[1].type);
    CHECK_EQUAL("1", tokens[1].text);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[2].type);
    CHECK_EQUAL(csv::util::PLUS, tokens[3].type);
}

TEST(tokenizer_rpn, test_simple_unary_minus_01) {
    std::istringstream is{"=-1"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(2, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::UNARY_MINUS, tokens[1].type);
}

TEST(tokenizer_rpn, test_simple_unary_plus_01) {
    std::istringstream is{"=+1"};
    std::vector<csv::util::token> tokens;

    csv::util::tokenize(is, tokens);
    csv::util::convert_to_rpn(tokens);

    CHECK_EQUAL(2, tokens.size());
    CHECK_EQUAL(csv::util::CONST, tokens[0].type);
    CHECK_EQUAL("1", tokens[0].text);
    CHECK_EQUAL(csv::util::UNARY_PLUS, tokens[1].type);
}
