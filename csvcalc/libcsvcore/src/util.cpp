#include "util.h"
#include "exceptions.h"

#include <stack>
#include <sstream>

namespace {

int op_priority(csv::util::TOKEN_TYPE type) {
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

}

namespace csv {

namespace util {

void tokenize(std::istream &is, std::vector<token> &result) {
    result.clear();

    while(std::isblank(is.peek())) {
        is.get();
    }
    
    if(is.peek() != '=') {
        std::string cval;
        while(true) {
            char sym = is.get();
            if(is.eof()) break;

            cval += sym;
        }
        if(cval.length() > 0) {
            result.push_back(token(CONST, cval));
        }
    } else {
        is.get();
        while(true) {
            char sym = is.get();
            if(is.eof()) break;
            if(std::isspace(sym)) continue;
            
            if(std::isalpha(sym)) {
                std::string ref;
                ref += sym;
                while(std::isalnum(is.peek())) {
                    ref += is.get();
                }
                result.push_back(token(CELL_REF, ref));
            } else if(std::isdigit(sym)) {
                std::string cval;
                cval += sym;
                while(std::isdigit(is.peek()) || ('.' == is.peek())) {
                    cval += is.get();
                }
                result.push_back(token(CONST, cval));
            } else {
                switch(sym) {
                case '(': result.push_back(token(OPEN_PAR)); break;
                case ')': result.push_back(token(CLOSE_PAR)); break;
                case '+': result.push_back(token(PLUS)); break;
                case '-': result.push_back(token(MINUS)); break;
                case '*': result.push_back(token(MULTIPLY)); break;
                case '/': result.push_back(token(DIVIDE)); break;
                default:
                    throw unsupported_op(sym);
                }
            }
        }
    }
}

void tokenize(const std::string &s, std::vector<token> &result) {
    std::istringstream is(s);
    tokenize(is, result);
}

void convert_to_rpn(std::vector<token> &tokens) {
    std::vector<token> result;
    std::stack<token> ops;

    bool op_seen = true;
    for(auto &t : tokens) {
        token t2;
        switch(t.type) {
        case CONST:
        case CELL_REF:
            result.push_back(t);
            op_seen = false;
            break;
        case OPEN_PAR:
            ops.push(t);
            op_seen = true;
            break;
        case CLOSE_PAR:
            while(!ops.empty()) {
                t2 = ops.top();
                ops.pop();
                if(OPEN_PAR == t2.type) break;

                result.push_back(t2);
            }
            if(OPEN_PAR != t2.type) {
                throw syntax_error("Parenthesis mismatch");
            }
            op_seen = false;
            break;
        case UNARY_PLUS:
        case PLUS:
        case UNARY_MINUS:
        case MINUS:
        case MULTIPLY:
        case DIVIDE:
            if(!op_seen && !ops.empty() && (::op_priority(ops.top().type) >= ::op_priority(t.type))) {
                result.push_back(ops.top());
                ops.pop();
            }
            if(op_seen) {
                if(PLUS == t.type) {
                    t.type = UNARY_PLUS;
                } else if(MINUS == t.type) {
                    t.type = UNARY_MINUS;
                }
            } else {
                op_seen = true;
            }
            ops.push(t);
            break;
        }
    }
    while(!ops.empty()) {
        if(OPEN_PAR == ops.top().type) {
            throw syntax_error("Parenthesis mismatch");
        }
        result.push_back(ops.top());
        ops.pop();
    }
    tokens = std::move(result); 
}

} // namespace util

} // namespace csv
