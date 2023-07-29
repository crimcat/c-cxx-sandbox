#include "ref.h"
#include "exceptions.h"

#include <cctype>
#include <iostream>
#include <sstream>

namespace csv {

ref ref::extract(std::istream &is) {
    if(is.bad() || is.fail()) {
        throw bad_or_failed_input_stream();
    }

    std::string tmp;
    size_t tmp_pos = 0;

    while(std::isalpha(is.peek())) {
        tmp += is.get();
        ++tmp_pos;
    }

    if(!tmp_pos) {
        throw bad_cell_address();
    }

    while(std::isdigit(is.peek())) {
        if(is.eof()) {
            break;
        }
        tmp += is.get();
    }

    if(tmp.length() == tmp_pos) {
        throw bad_cell_address();
    }

    return ref(tmp, tmp_pos);
}

ref ref::extract(const std::string &s) {
    std::string tmp;

    auto it = s.begin();
    while((s.end() != it) && std::isalpha(*it)) {
        tmp += *it;
        ++it;
    }
    const size_t tmp_pos = std::distance(s.begin(), it);
    if(0 == tmp_pos) {
        throw bad_cell_address(s);
    }

    while((s.end() != it) && std::isdigit(*it)) {
        tmp += *it;
        ++it;
    }
    if(tmp.length() == tmp_pos) {
        throw bad_cell_address(s);
    }

    return ref(tmp, tmp_pos);
}


} // namespace csv
