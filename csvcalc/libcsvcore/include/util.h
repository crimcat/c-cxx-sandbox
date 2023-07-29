#pragma once

#include "exceptions.h"

#include <string>
#include <vector>
#include <iostream>

namespace csv {

namespace util {

/**
 * @brief Types of recognized tokens 
 */
enum TOKEN_TYPE {
    CONST,
    CELL_REF,
    OPEN_PAR,
    CLOSE_PAR,
    UNARY_PLUS,
    PLUS,
    UNARY_MINUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    UNDEF
};

/**
 * @brief Token representation after the tokenizer
 */
struct token {
    TOKEN_TYPE type;
    std::string text;

    token() : type(UNDEF) {}
    token(TOKEN_TYPE t) : type(t) {}
    token(TOKEN_TYPE t, const std::string &s) : type(t), text(s) {}

    /**
     * @brief Is operation unary
     */
    inline bool is_unary_op() const {
        return (UNARY_PLUS == type) || (UNARY_MINUS == type);
    }

    /**
     * @brief Is operation binary
     */
    inline bool is_binary_op() const {
        return (PLUS == type) || (MINUS == type) || (MULTIPLY == type) || (DIVIDE == type);
    }

    /**
     * @brief Get operation priority if applicable
     * Returns 0 for non-operation tokens.
     */
    inline int priority() const {
        switch(type) {
        case csv::util::PLUS:
        case csv::util::MINUS:
            return 1;
        case csv::util::MULTIPLY:
        case csv::util::DIVIDE:
            return 2;
        case csv::util::UNARY_PLUS:
        case csv::util::UNARY_MINUS:
            return 3;
        default:
            return 0;
        }
    }
};

/**
 * @brief Tokenize input stream
 * Result vector is cleared before adding acquired tokens.
 * 
 * @param is input stream
 * @param result vector of tokens to return to the caller
 */
void tokenize(std::istream &is, std::vector<token> &result);

/**
 * @brief Tokenized input string
 * Result vector is cleared before adding acquired tokens.
 * 
 * @param s input string
 * @param result vector of tokens to return to the caller
 */
void tokenize(const std::string &s, std::vector<token> &result);

/**
 * @brief Convert vector of tokens into the RPN notation ready to calculate
 * Used for reference only.
 * 
 * @param tokens input/output vector of tokens
 */
void convert_to_rpn(std::vector<token> &tokens);

} // namespace util

} // namespace csv
