#pragma once

#include <string>

namespace csv {

/**
 * @brief Table cell holder
 * Class instance containes cell as text, calculated value if available,
 * flag to indicate that cell is locked till it's evaluated and flag to indicate that the cell value is ready.
 */
template<typename V>
class cell {
public:
    cell() = default;

    /**
     * @brief Construct a new cell object
     * 
     * @param as_text cell text to evaluate
     */
    explicit cell(const std::string &as_text) : _as_text(as_text) {}

    /**
     * @brief Get cell content as string
     * 
     * @return std::string_view with cell text
     */
    const std::string &to_string() const {
        return _as_text;
    }

    /**
     * @brief Is cell value calculated and assigned
     * 
     * @return true 
     * @return false 
     */
    bool is_assigned() const {
        return _assigned;
    }

    /**
     * @brief Set cell value and remove cell lock flag
     * 
     * @param v calculated value
     */
    void assign_and_unlock(V v) {
        _value = v;
        _assigned = true;
        _locked = false;
    }

    /**
     * @brief Get cell value
     * 
     * @return cell value or default type value if cell isn't assignbed
     */
    V value() const {
        return _value;
    }

    /**
     * @brief Lock the cell
     */
    void lock() {
        _locked = true;
    }

    /**
     * @brief Is cell locked
     * 
     * @return true of locked (under evaluation)
     */
    bool is_locked() const {
        return _locked;
    }

private:
    std::string _as_text;
    bool _assigned{false};
    bool _locked{false};
    V _value{};
};

} // namespace csv
