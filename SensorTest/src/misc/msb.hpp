#pragma once

#include <array>

namespace misc
{
    /// @brief Yields the position (power of 2) of the most significant bit of an integer
    /// @tparam T Numeric type
    /// @param number Number value
    /// @return Bit position
    template<typename T>
    constexpr std::size_t msb(T number)
    {
        static_assert(std::numeric_limits<T>::is_integer, "'T' must be an integer");

        std::size_t i = 0;
        for(; number != 0; i++)
        {
            number >>= 1;
        }
        return i;
    }
}