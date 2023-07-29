#pragma once

#include "ref.h"
#include "cell.h"

#include <string>
#include <unordered_map>

namespace csv {

inline bool operator==(const ref &r1, const ref &r2) {
    return r1.to_string() == r2.to_string();
}

struct ref_hash {
    std::size_t operator()(const ref &r) const noexcept {
        return std::hash<std::string>()(r.to_string());
    }
};

/**
 * @brief Lookup table definition
 */
template<typename V>
using lookup_table = std::unordered_map<ref, cell<V>, ref_hash>;

} // namespace csv
