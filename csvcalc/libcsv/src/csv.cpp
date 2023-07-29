#include "csv.h"

#include <cctype>

namespace {

void skip_spaces(std::istream &is) {
    while(std::isspace(is.peek())) {
        is.get();
    }
}

void skip_blanks(std::istream &is) {
    while(std::isblank(is.peek())) {
        is.get();
    }
}

bool is_eol(std::istream &is) {
    int cnt = 0;
    char c = is.get();
    while(!is.eof() && (('\n' == c) || ('\r' == c))) {
        c = is.get();
        ++cnt;
    }
    if(!is.eof()) {
        is.putback(c);
        return cnt > 0;
    }
    return true;
}

void extract_delimiter(std::istream &is) {
    if(is.peek() != ',') {
        throw csv::missing_delimiter();
    }
    is.get();
}

void extract_column_name(std::istream &is, std::string &col) {
    col.clear();
    while(!is.eof() && std::isalpha(is.peek())) {
        col += is.get();
    }
    if(col.size() == 0) {
        throw csv::illformed_column_name();
    }
}

void extract_row_name(std::istream &is, std::string &row) {
    row.clear();
    while(!is.eof() && std::isdigit(is.peek())) {
        row += is.get();
    }
    if(row.size() == 0) {
        throw csv::illformed_row_name();
    }
}

void extract_cell(std::istream &is, std::string &cell) {
    cell.clear();
    while(!is.eof() && (std::isblank(is.peek()) || std::isalnum(is.peek()) || std::ispunct(is.peek()))) {
        if(',' == is.peek()) break;
        cell += is.get();
    }
}



} // namespace

namespace csv {

void read(std::istream &is, data &d) {
    if(is.bad() || is.fail()) {
        throw bad_or_failed_input_stream();
    }

    d.rows.clear();
    d.columns.clear();
    d.raw_csv.clear();

    ::skip_spaces(is);

    // read CSV header
    while(true) {
        ::skip_blanks(is);
        if(::is_eol(is)) break;

        ::extract_delimiter(is);
        ::skip_blanks(is);

        std::string col;
        ::extract_column_name(is, col);
        d.columns.push_back(col);
    }

    // read rows iteratively
    while(!is.eof()) {
        ::skip_blanks(is);

        std::string row;
        ::extract_row_name(is, row);
        d.rows.push_back(row);

        int col_number = 0;
        while(true) {
            ::skip_blanks(is);
            if(::is_eol(is)) break;

            ::extract_delimiter(is);

            std::string cell;
            ::extract_cell(is, cell);

            d.raw_csv[d.columns[col_number] + row] = cell;
            ++col_number;
        }
        if(d.columns.size() != col_number) {
            throw irregular_rows();
        }
    }
}

} // namespace csv
