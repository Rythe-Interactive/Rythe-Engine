#include <cstdint>
#include <cstddef>

/** @file unit_conversion.hpp
 */

namespace legion::core {
namespace literals 
{
    constexpr std:: uint64_t onek = 1024;

    /** @brief gets X amount of kb in bytes */
    constexpr std:: uint64_t operator""_kb(unsigned long long param) {
        return param * onek;
    }

    /** @brief gets X amount of mb in bytes */
    constexpr std:: uint64_t operator""_mb(unsigned long long param) {
        return param * onek * onek;
    }

    /** @brief gets X amount of gb in bytes */
    constexpr std:: uint64_t operator""_gb(unsigned long long param) {
        return param * onek * onek * onek;
    }
}
}
