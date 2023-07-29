#pragma once

#include "libcsvcore/include/calculator.h"

#include <iostream>
#include <vector>

namespace csv {

/**
 * @brief Data structure to accumulate information about CSV file
 * Contains:
 * - array of row names in natural order
 * - array of column names in natural order
 * - hashtable of mapping between all cell addresses and raw cell contents
 */
struct data {
    std::vector<std::string> rows;
    std::vector<std::string> columns;
    std::unordered_map<std::string, std::string> raw_csv;
};

/**
 * @brief Read CSV data from input stream into the CVS representaion structure
 * 
 * @param is input stream
 * @param d CSV data
 */
void read(std::istream &is, data &d);

/**
 * @brief CSV table view
 * 
 * @tparam V CSV expression type
 */
template<typename V>
class table {
public:
    /**
     * @brief Extract CSV data from input stream and create CSV table view
     * 
     * @param is input stream
     * @return table view object
     */
    static table extract(std::istream &is) {
        table tbl;
        data d;
        read(is, d);
        
        for(auto &e : d.raw_csv) {
            tbl._lut[ref::extract(e.first)] = std::move(cell<V>(e.second));
        }
        tbl._rows = std::move(d.rows);
        tbl._columns = std::move(d.columns);

        return std::move(tbl);
    }

    /**
     * @brief Get CSV row names in natural order
     * 
     * @return vector with row names, can be empty if CSV contains no rows
     */
    const std::vector<std::string> &rows() const {
        return _rows;
    }

    /**
     * @brief Get CSV column names in natural order
     * 
     * @return vector with column names, can be empty if CSV contains no columns
     */
    const std::vector<std::string> &columns() const {
        return _columns;
    }

    /**
     * @brief Evaluate and get cell value
     * 
     * @param cell_address cell address to evaluate expression in
     * @return expression value
     */
    V cell_value(const std::string &cell_address) const {
        auto it = _lut.find(ref::extract(cell_address));
        if(_lut.end() == it) {
            throw bad_cell_address(cell_address);
        }
        return calc::expression_calc(it->second, _lut)();
    }

    /**
     * @brief Evaluate and get cell value using separate row and column names
     * 
     * @param row row name
     * @param col column name
     * @return expression value
     */
    V cell_value(const std::string &row, const std::string &col) const {
        return cell_value(col + row);
    }

    /**
     * @brief Get cell as text
     * 
     * @param cell_address cell address
     * @return cell text as it's provided in the source CSV, all spaces preserved
     */
    const std::string &cell_text(const std::string &cell_address) const {
        auto it = _lut.find(ref::extract(cell_address));
        if(_lut.end() == it) {
            throw bad_cell_address(cell_address);
        }
        return it->second.to_string();
    }

    /**
     * @brief Get cell as text using separate row and column names
     * 
     * @param row row name
     * @param col column name
     * @return cell text as it's provided in the source CSV, all spaces preserved
     */
    const std::string &cell_text(const std::string &row, const std::string &col) const {
        return cell_text(col + row);
    }

private:
    table() = default;

    mutable lookup_table<V> _lut;
    std::vector<std::string> _rows;
    std::vector<std::string> _columns;
};

} // namespace csv
