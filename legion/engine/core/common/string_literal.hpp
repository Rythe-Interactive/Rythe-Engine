#pragma once
#include <array>

#include <core/types/types.hpp>

namespace legion::core
{
    template<size_type N>
    struct string_literal
    {
        const char(&value)[N];

        constexpr string_literal(const char(&str)[N]) noexcept : value(str) {}

        constexpr string_literal(const string_literal& src) noexcept : value(src.value) {}
        constexpr string_literal(string_literal&& src) noexcept : value(src.value) {}

        constexpr string_literal& operator=(const string_literal& src) noexcept { value = src.value; }
        constexpr string_literal& operator=(string_literal&& src) noexcept { value = src.value; }

        constexpr size_type length() const noexcept { return N; }

        constexpr operator cstring () const noexcept { return value; }
        constexpr operator const std::array<char, N>() const noexcept { return value; }
    };


#if !defined(DOXY_EXCLUDE)
    template<size_type N>
    string_literal(const char(&)[N])->string_literal<N>;
#endif
}
