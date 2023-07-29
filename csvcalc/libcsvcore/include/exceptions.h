#pragma once

#include <stdexcept>

namespace csv {

/**
 * @brief Input stream is failed or bad
 */
class bad_or_failed_input_stream : public std::exception { };

/**
 * @brief Indication that rows are not of the same size
 */
class irregular_rows : public std::exception { };

/**
 * @brief Common exception for CSV parsing issues
 */
class parse_exception : public std::exception { };

/**
 * @brief Expected cell delimiter is not met
 */
class missing_delimiter : public parse_exception {
public:
    const char *what() const noexcept override {
        return "Parser error: missing delimiter";
    }
};

/**
 * @brief Column name is not correct
 */
class illformed_column_name : public parse_exception {
public:
    const char *what() const noexcept override {
        return "Parser error: bad column name";
    }
};

/**
 * @brief Row name is not correct
 */
class illformed_row_name : public parse_exception {
public:
    const char *what() const noexcept override {
        return "Parser error: bad row name";
    }
};

/**
 * @brief Base exception class for expression evaluation
 */
class eval_exception : public std::runtime_error {
public:
    explicit eval_exception(const std::string &msg) : std::runtime_error(msg) { }
};

/**
 * @brief Unsupported operation sign is used
 */
class unsupported_op : public eval_exception {
public:
    unsupported_op(char sym) : eval_exception(std::string("Unknown operation: ") + std::string(1, sym)) { }
};

/**
 * @brief Cannot resolve cell address in the lookup table
 */
class bad_cell_address : public eval_exception {
public:
    bad_cell_address() : eval_exception("") { }
    explicit bad_cell_address(const std::string &addr) : eval_exception("Bad cell address: " + addr) { }
};

/**
 * @brief Syntax error in expression
 */
class syntax_error : public eval_exception {
public:
    explicit syntax_error(const std::string &msg) : eval_exception("Syntax error: " + msg) { }
};

/**
 * @brief Cyclic references between cells in CSV
 */
class cyclic_reference : public eval_exception {
public:
    explicit cyclic_reference(const std::string &msg) : eval_exception("Infinite looping: " + msg) { }
};

/**
 * @brief Division by zero during expression evaluation
 */
class division_by_zero : public eval_exception {
public:
    division_by_zero() : eval_exception("") { }
    explicit division_by_zero(const std::string &msg) : eval_exception("Division by zero: " + msg) { }
};

/**
 * @brief Internal error - algorithm came to a point where it should not appear
 */
class implementation_bug : public std::exception { };

} // namespace csv
