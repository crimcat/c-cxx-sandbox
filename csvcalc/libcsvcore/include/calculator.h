#pragma once

#include "util.h"
#include "lut.h"
#include "exceptions.h"

#include <string>
#include <vector>
#include <stack>

namespace csv {

namespace calc {

/**
 * @brief Common decoder template
 * 
 * @tparam V value type
 * @param s string to convert to V
 * @return conversion result if possible
 */
template<typename V>
inline V from_string(const std::string &s);

/**
 * @brief Specialization for int
 */
template<>
inline int from_string(const std::string &s) {
    try {
        std::size_t pos;
        int res = std::stoi(s, &pos);
        if(s.length() != pos) {
            throw syntax_error(s);
        }
        return res;
    } catch(const std::invalid_argument &) {
        throw syntax_error(s);
    }
}

/**
 * @brief Specialization for double
 */
template<>
inline double from_string(const std::string &s) {
    try {
        std::size_t pos;
        double res = std::stod(s, &pos);
        if(s.length() != pos) {
            throw syntax_error(s);
        }
        return res;
    } catch(...) {
        throw syntax_error(s);
    }
}

/**
 * @brief Unary plus operation or does nothing
 */
template<typename V>
V unary_plus(V arg) {
    return arg;
}

/**
 * @brief Plus operation
 */
template<typename V>
V plus(V arg1, V arg2) {
    return arg1 + arg2;
}

/**
 * @brief Unary minus or negation
 */
template<typename V>
V unary_minus(V arg1) {
    return -arg1;
}

/**
 * @brief Subtraction
 */
template<typename V>
V minus(V arg1, V arg2) {
    return arg1 - arg2;
}

/**
 * @brief Multiplication operation
 */
template<typename V>
V multiply(V arg1, V arg2) {
    return arg1 * arg2;
}

/**
 * @brief Division operation
 * Can throw exception in case zero second argument.
 */
template<typename V>
V divide(V arg1, V arg2) {
    if(V(0) == arg2) {
        throw division_by_zero();
    }
    return arg1 / arg2;
}

/**
 * @brief Templace class for expression calcuation
 * Implements functor paradigm. Expression arguments and result must be of the same type V.
 */
template<typename V>
class expression_calc {
public:
    /**
     * @brief Construct a new expression calc object
     * 
     * @param cell cell containing the expression
     * @param lut lookup table to search values of other cells
     */
    expression_calc(csv::cell<V> &cell, csv::lookup_table<V> &lut) : _cell(&cell), _lut(&lut) {}

    /**
     * @brief Run expression evaluation.
     * Implementation relies on tokenizer.
     * 
     * @see csv::tokenize()
     * @return expression value
     */
    V operator()() {
        if(!_cell->is_assigned()) {
            _cell->lock();
            std::vector<util::token> tokenized_expression;
            util::tokenize(_cell->to_string(), tokenized_expression);
            if(tokenized_expression.size() > 0) {
                for(auto &t : tokenized_expression) {
                    switch(t.type) {
                    case util::CONST: process_const(t); break;
                    case util::CELL_REF: process_cell_ref(t); break;
                    case util::OPEN_PAR: process_open_par(t); break;
                    case util::CLOSE_PAR: process_close_par(t); break;
                    case util::UNARY_PLUS:  /* fall further */
                    case util::PLUS:        /* fall further */
                    case util::UNARY_MINUS: /* fall further */
                    case util::MINUS:       /* fall further */
                    case util::MULTIPLY:    /* fall further */
                    case util::DIVIDE: process_arithmetic_op(t); break;
                    default:
                        throw implementation_bug();
                    }
                }
                while(!ops.empty()) {
                    if(util::OPEN_PAR == ops.top().type) {
                        throw syntax_error(_cell->to_string());
                    }
                    calculate_op(ops.top());
                    ops.pop();
                }
                if(result.size() > 1) {
                    throw syntax_error(_cell->to_string());
                }
                _cell->assign_and_unlock(result.back());
            }
        }
        return _cell->value();
    }

private:
    /**
     * @brief Process const token
     */
    void process_const(const util::token &tok) {
        result.push_back(from_string<V>(tok.text));
        op_seen = false;
    }

    /**
     * @brief Process cell reference (cell address in the lookup table)
     */
    void process_cell_ref(const util::token &tok) {
        auto it = _lut->find(ref::extract(tok.text));
        if(_lut->end() == it) {
            throw bad_cell_address(tok.text);
        }
        if(it->second.is_locked()) {
            throw cyclic_reference(_cell->to_string());
        }
        result.push_back(expression_calc(it->second, *_lut)());
        op_seen = false;
    }

    /**
     * @brief Process open parenthesis token
     */
    void process_open_par(const util::token &tok) {
        ops.push(tok);
        op_seen = true;
    }

    /**
     * @brief Process close parenthesis token
     * Actually, evaluate the expression till the open parenthesis.
     */
    void process_close_par(const util::token &tok) {
        util::token t2;
        while(!ops.empty()) {
            t2 = ops.top();
            ops.pop();
            if(util::OPEN_PAR == t2.type) break;

            calculate_op(t2);
        }
        if(util::OPEN_PAR != t2.type) {
            throw syntax_error(_cell->to_string());
        }
        op_seen = false;
    }

    /**
     * @brief Process arithmetic operation token
     * Perform calculation or stack push according to the operation priorities.
     */
    void process_arithmetic_op(const util::token &tok) {
        if(!op_seen) {
            while(!ops.empty() && (ops.top().priority() >= tok.priority())) {
                calculate_op(ops.top());
                ops.pop();
            }
        }
        if(op_seen) {
            if(util::PLUS == tok.type) {
                ops.push(util::token(util::UNARY_PLUS));
            } else if(util::MINUS == tok.type) {
                ops.push(util::token(util::UNARY_MINUS));
            } else {
                throw syntax_error("only unary plus and minus allowed");
            }
        } else {
            ops.push(tok);
            op_seen = true;
        }
    }

    /**
     * @brief Calculate arithmetic operation
     * Takes operands from stack, pushes result back to stack.
     */
    void calculate_op(const util::token &tok) {
        if(tok.is_unary_op()) {
            if(result.size() < 1) {
                throw syntax_error(_cell->to_string());
            }
        } else if(tok.is_binary_op()) {
            if(result.size() < 2) {
                throw syntax_error(_cell->to_string());
            }
        } else {
            throw implementation_bug();
        }

        V arg1 = result.back();
        result.pop_back();
        V arg2;
        if(tok.is_binary_op()) {
            arg2 = result.back();
            result.pop_back();
        }
        switch(tok.type) {
        case util::UNARY_PLUS: result.push_back(unary_plus(arg1)); break;
        case util::UNARY_MINUS: result.push_back(unary_minus(arg1)); break;
        case util::PLUS: result.push_back(plus(arg2, arg1)); break;
        case util::MINUS: result.push_back(minus(arg2, arg1)); break;
        case util::MULTIPLY: result.push_back(multiply(arg2, arg1)); break;
        case util::DIVIDE:
            try {
                result.push_back(divide(arg2, arg1));
            } catch(const division_by_zero &e) {
                throw division_by_zero(_cell->to_string());
            }
            break;
        default:
            throw implementation_bug();
        }
    }

    cell<V> *_cell;
    lookup_table<V> *_lut;
    
    std::vector<V> result{};
    std::stack<util::token> ops{};

    bool op_seen{true}; // flag to detect unary minus and plus, indicates that operation sign has just been processed
};

} // namespace calc

} // namespace csv
