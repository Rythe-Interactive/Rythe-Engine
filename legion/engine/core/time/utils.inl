#include <core/time/utils.hpp>
#pragma once

namespace legion::core::time
{
    template<typename precision>
    constexpr time_span<precision> hours(precision amount) noexcept
    {
        int64 ratio = 3600;
        int64 integer = 0;

        if constexpr (std::numeric_limits<precision>::is_iec559)
        {
            precision integral = 0;
            precision fractional = math::modf(amount, integral);
            // Avoid floating point precision loss with large numbers by multiplying the integer
            // part of the floating point number seperately using integer multiplication.
            integer = math::iround<precision, int64>(integral) * ratio + math::iround<precision, int64>(fractional * ratio);
        }
        else
        {
            integer = amount * ratio;
        }

        return time_span<precision>(seconds(integer));
    }

    template<typename precision>
    constexpr time_span<precision> minutes(precision amount) noexcept
    {
        int64 ratio = 60000;
        int64 integer = 0;

        if constexpr (std::numeric_limits<precision>::is_iec559)
        {
            precision integral = 0;
            precision fractional = math::modf(amount, integral);
            // Avoid floating point precision loss with large numbers by multiplying the integer
            // part of the floating point number seperately using integer multiplication.
            integer = math::iround<precision, int64>(integral) * ratio + math::iround<precision, int64>(fractional * ratio);
        }
        else
        {
            integer = amount * ratio;
        }

        return time_span<precision>(milliseconds(integer));
    }

    template<typename precision>
    constexpr time_span<precision> seconds(precision amount) noexcept
    {
        int64 ratio = 1000000;
        int64 integer = 0;

        if constexpr (std::numeric_limits<precision>::is_iec559)
        {
            precision integral = 0;
            precision fractional = math::modf(amount, integral);
            // Avoid floating point precision loss with large numbers by multiplying the integer
            // part of the floating point number seperately using integer multiplication.
            integer = math::iround<precision, int64>(integral) * ratio + math::iround<precision, int64>(fractional * ratio);
        }
        else
        {
            integer = amount * ratio;
        }

        return time_span<precision>(microseconds(integer));
    }

    template<typename precision>
    constexpr time_span<precision> milliseconds(precision amount) noexcept
    {
        int64 ratio = 1000000;
        int64 integer = 0;

        if constexpr (std::numeric_limits<precision>::is_iec559)
        {
            precision integral = 0;
            precision fractional = math::modf(amount, integral);
            // Avoid floating point precision loss with large numbers by multiplying the integer
            // part of the floating point number seperately using integer multiplication.
            integer = math::iround<precision, int64>(integral) * ratio + math::iround<precision, int64>(fractional * ratio);
        }
        else
        {
            integer = amount * ratio;
        }

        return time_span<precision>(std::chrono::nanoseconds(integer));
    }

    template<typename precision>
    constexpr time_span<precision> microseconds(precision amount) noexcept
    {
        int64 ratio = 1000;
        int64 integer = 0;

        if constexpr (std::numeric_limits<precision>::is_iec559)
        {
            precision integral = 0;
            precision fractional = math::modf(amount, integral);
            // Avoid floating point precision loss with large numbers by multiplying the integer
            // part of the floating point number seperately using integer multiplication.
            integer = math::iround<precision, int64>(integral) * ratio + math::iround<precision, int64>(fractional * ratio);
        }
        else
        {
            integer = amount * ratio;
        }

        return time_span<precision>(std::chrono::nanoseconds(integer));
    }

    template<typename precision>
    constexpr time_span<precision> nanoseconds(precision amount) noexcept
    {
        if constexpr (std::numeric_limits<precision>::is_iec559)
            return time_span<precision>(std::chrono::nanoseconds(math::iround<precision, int64>(amount)));
        else
            return time_span<precision>(std::chrono::nanoseconds(static_cast<int64>(amount)));
    }

    namespace literals
    {
        constexpr span operator""_hr(float_max amount) noexcept
        {
            return hours<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_hr(uint_max amount) noexcept
        {
            return hours<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_min(float_max amount) noexcept
        {
            return minutes<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_min(uint_max amount) noexcept
        {
            return minutes<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_s(float_max amount) noexcept
        {
            return seconds<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_s(uint_max amount) noexcept
        {
            return seconds<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_ms(float_max amount) noexcept
        {
            return milliseconds<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_ms(uint_max amount) noexcept
        {
            return milliseconds<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_us(float_max amount) noexcept
        {
            return microseconds<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_us(uint_max amount) noexcept
        {
            return microseconds<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_ns(float_max amount) noexcept
        {
            return nanoseconds<time32>(static_cast<time32>(amount));
        }

        constexpr span operator""_ns(uint_max amount) noexcept
        {
            return nanoseconds<time32>(static_cast<time32>(amount));
        }
    }
}
