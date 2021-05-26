#pragma once
#include <core/time/time_span.hpp>

namespace legion::core::time
{
    template<typename precision = fast_time>
    constexpr time_span<precision> hours(precision amount) noexcept;

    template<typename precision = fast_time>
    constexpr time_span<precision> minutes(precision amount) noexcept;

    template<typename precision = fast_time>
    constexpr time_span<precision> seconds(precision amount) noexcept;

    template<typename precision = fast_time>
    constexpr time_span<precision> milliseconds(precision amount) noexcept;

    template<typename precision = fast_time>
    constexpr time_span<precision> microseconds(precision amount) noexcept;

    template<typename precision = fast_time>
    constexpr time_span<precision> nanoseconds(precision amount) noexcept;

    namespace literals
    {
        constexpr span operator "" _hr(f80 amount) noexcept;
        constexpr span operator "" _hr(uint64 amount) noexcept;

        constexpr span operator "" _min(f80 amount) noexcept;
        constexpr span operator "" _min(uint64 amount) noexcept;

        constexpr span operator "" _s(f80 amount) noexcept;
        constexpr span operator "" _s(uint64 amount) noexcept;

        constexpr span operator "" _ms(f80 amount) noexcept;
        constexpr span operator "" _ms(uint64 amount) noexcept;

        constexpr span operator "" _us(f80 amount) noexcept;
        constexpr span operator "" _us(uint64 amount) noexcept;

        constexpr span operator "" _ns(f80 amount) noexcept;
        constexpr span operator "" _ns(uint64 amount) noexcept;
    }
}

#include <core/time/utils.inl>
