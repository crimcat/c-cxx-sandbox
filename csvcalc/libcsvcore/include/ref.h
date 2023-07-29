#pragma once

#include <string>
#include <string_view>
#include <iostream>

namespace csv {

class ref {
public:
    /**
     * @brief Extract cell references from text
     * In case of errors raises exceptions:
     * - bad_or_failed_input_stream - error reading data from input stream
     * - bad_cell_address - cannot parse cell address
     * 
     * @param is input stream or string
     */
    static ref extract(std::istream &is);
    static ref extract(const std::string &s);

    /**
     * @brief Get column name
     * 
     * @return std::string_view with column name 
     */
    std::string_view column() const {
        return _as_text.length() ? std::string_view(_as_text).substr(_split_pos) : "";
    }
    
    /**
     * @brief Get row name
     * 
     * @return std::string_view with row name 
     */
    std::string_view row() const {
        return _as_text.length() ? std::string_view(_as_text).substr(0, _split_pos) : "";
    }

    /**
     * @brief Get address as is
     * 
     * @return address
     */
    const std::string &to_string() const {
        return _as_text;
    }

    ref() = default;
private:
    /**
     * @brief Construct a new ref object, for internal use
     * 
     * @param s reference string
     * @param p position of row part in this reference string
     */
    ref(const std::string &s, size_t p) : _as_text(s), _split_pos(p) {}

    std::string _as_text;
    size_t _split_pos;
};

/**
 * @brief stream operator overload 
 * In case of errors raises exceptions:
 * - parse_exception - severe parser error
 * - bad_row_name - row name cannot be parsed
 * - bad_column_name - column name cannot be parsed
 * 
 * @param is input stream
 * @param obj reference
 * @return std::istream& for chaining
 */
inline std::istream &operator>>(std::istream &is, ref &obj) {
    obj = std::move(obj.extract(is));
    return is;
}

} // namespace cvs2
