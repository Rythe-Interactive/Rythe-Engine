#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/math/vector/vector_base.hpp>

namespace legion::core::math::detail
{
    template<typename _Scalar, size_type _Size, size_type... args>
    struct swizzle;

    template<typename _Scalar, size_type _Size, size_type s0, size_type s1>
    struct swizzle<_Scalar, _Size, s0, s1>
    {
        using scalar = _Scalar;
        static constexpr size_type size = _Size;
        using type = swizzle<_Scalar, _Size, s0, s1>;
        using conv_type = vector<_Scalar, 2>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr operator conv_type() const noexcept { return conv_type(data[s0], data[s1]); }
        constexpr swizzle& operator=(const conv_type& other) noexcept
        {
            data[s0] = other[0]; data[s1] = other[1];
            return *this;
        }
    };

    template<typename _Scalar, size_type _Size, size_type s0, size_type s1, size_type s2>
    struct swizzle<_Scalar, _Size, s0, s1, s2>
    {
        using scalar = _Scalar;
        static constexpr size_type size = _Size;
        using type = swizzle<_Scalar, _Size, s0, s1, s2>;
        using conv_type = vector<_Scalar, 3>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr operator conv_type() const noexcept { return conv_type(data[s0], data[s1], data[s2]); }
        constexpr swizzle& operator=(const conv_type& other) noexcept
        {
            data[s0] = other[0]; data[s1] = other[1]; data[s2] = other[2];
            return *this;
        }
    };

    template<typename _Scalar, size_type _Size, size_type s0, size_type s1, size_type s2, size_type s3>
    struct swizzle<_Scalar, _Size, s0, s1, s2, s3>
    {
        using scalar = _Scalar;
        static constexpr size_type size = _Size;
        using type = swizzle<_Scalar, _Size, s0, s1, s2, s3>;
        using conv_type = vector<_Scalar, 4>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr operator conv_type() const noexcept { return conv_type(data[s0], data[s1], data[s2], data[s3]); }
        constexpr swizzle& operator=(const conv_type& other) noexcept
        {
            data[s0] = other[0]; data[s1] = other[1]; data[s2] = other[2]; data[s3] = other[3];
            return *this;
        }
    };
}
