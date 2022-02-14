#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/math/vector/vector_base.hpp>

namespace legion::core::math::detail
{
    template<typename _Scalar, size_type _Size, size_type... args>
    struct swizzle;

    template<typename _Scalar, size_type _Size, size_type s0>
    struct swizzle<_Scalar, _Size, s0>
    {
        using scalar = _Scalar;
        static constexpr size_type size = 1;
        using type = swizzle<_Scalar, _Size, s0>;
        using conv_type = vector<scalar, 1>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x) noexcept { data[s0] = x; }

        constexpr operator scalar() const noexcept { return data[s0]; }

        constexpr operator conv_type() const noexcept
        {
            conv_type result;
            result.data[0] = data[s0];
            return result;
        }

        constexpr swizzle& operator=(const conv_type& other) noexcept
        {
            data[s0] = other[0];
            return *this;
        }

        constexpr swizzle& operator=(scalar value) noexcept
        {
            data[s0] = value;
            return *this;
        }
    };

    template<size_type _Size, size_type s0>
    struct swizzle<bool, _Size, s0>
    {
        using scalar = bool;
        static constexpr size_type size = 1;
        using type = swizzle<bool, _Size, s0>;
        using conv_type = vector<scalar, 1>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x) noexcept { data[s0] = x; }

        constexpr void set_mask(bitfield8 mask) noexcept { data[s0] = mask & 1; }
        constexpr bitfield8 mask() const noexcept { return static_cast<bitfield8>(data[s0]); }

        constexpr operator scalar() const noexcept { return data[s0]; }

        constexpr operator conv_type() const noexcept
        {
            conv_type result;
            result.data[0] = data[s0];
            return result;
        }

        constexpr swizzle& operator=(const conv_type& other) noexcept
        {
            data[s0] = other[0];
            return *this;
        }

        constexpr swizzle& operator=(scalar value) noexcept
        {
            data[s0] = value;
            return *this;
        }
    };

    template<typename _Scalar, size_type _Size, size_type s0, size_type s1>
    struct swizzle<_Scalar, _Size, s0, s1>
    {
        using scalar = _Scalar;
        static constexpr size_type size = 2;
        using type = swizzle<_Scalar, _Size, s0, s1>;
        using conv_type = vector<scalar, 2>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y) noexcept { data[s0] = x; data[s1] = y; }

        constexpr operator conv_type() const noexcept
        {
            conv_type result;
            result.data[0] = data[s0];
            result.data[1] = data[s1];
            return result;
        }

        constexpr swizzle& operator=(const conv_type& other) noexcept
        {
            data[s0] = other[0]; data[s1] = other[1];
            return *this;
        }
    };

    template<size_type _Size, size_type s0, size_type s1>
    struct swizzle<bool, _Size, s0, s1>
    {
        using scalar = bool;
        static constexpr size_type size = 2;
        using type = swizzle<bool, _Size, s0, s1>;
        using conv_type = vector<scalar, 2>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y) noexcept { data[s0] = x; data[s1] = y; }

        constexpr void set_mask(bitfield8 mask) noexcept { data[s0] = mask & 1; data[s1] = mask & 2; }
        constexpr bitfield8 mask() const noexcept { return static_cast<bitfield8>(data[s0] | data[s0] << 1u); }

        constexpr operator conv_type() const noexcept
        {
            conv_type result;
            result.data[0] = data[s0];
            result.data[1] = data[s1];
            return result;
        }

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
        static constexpr size_type size = 3;
        using type = swizzle<_Scalar, _Size, s0, s1, s2>;
        using conv_type = vector<scalar, 3>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y, scalar z) noexcept { data[s0] = x; data[s1] = y; data[s2] = z; }

        constexpr operator conv_type() const noexcept
        {
            conv_type result;
            result.data[0] = data[s0];
            result.data[1] = data[s1];
            result.data[2] = data[s2];
            return result;
        }

        constexpr swizzle& operator=(const conv_type& other) noexcept
        {
            data[s0] = other[0]; data[s1] = other[1]; data[s2] = other[2];
            return *this;
        }
    };

    template<size_type _Size, size_type s0, size_type s1, size_type s2>
    struct swizzle<bool, _Size, s0, s1, s2>
    {
        using scalar = bool;
        static constexpr size_type size = 3;
        using type = swizzle<bool, _Size, s0, s1, s2>;
        using conv_type = vector<scalar, 3>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y, scalar z) noexcept { data[s0] = x; data[s1] = y; data[s2] = z; }

        constexpr void set_mask(bitfield8 mask) noexcept { data[s0] = mask & 1; data[s1] = mask & 2; data[s2] = mask & 4; }
        constexpr bitfield8 mask() const noexcept { return static_cast<bitfield8>(data[s0] | data[s1] << 1u | data[s2] << 2u); }

        constexpr operator conv_type() const noexcept
        {
            conv_type result;
            result.data[0] = data[s0];
            result.data[1] = data[s1];
            result.data[2] = data[s2];
            return result;
        }

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
        static constexpr size_type size = 4;
        using type = swizzle<_Scalar, _Size, s0, s1, s2, s3>;
        using conv_type = vector<scalar, 4>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y, scalar z, scalar w) noexcept { data[s0] = x; data[s1] = y; data[s2] = z; data[s3] = w; }

        constexpr operator conv_type() const noexcept
        {
            conv_type result;
            result.data[0] = data[s0];
            result.data[1] = data[s1];
            result.data[2] = data[s2];
            result.data[3] = data[s3];
            return result;
        }

        constexpr swizzle& operator=(const conv_type& other) noexcept
        {
            data[s0] = other[0]; data[s1] = other[1]; data[s2] = other[2]; data[s3] = other[3];
            return *this;
        }
    };

    template<size_type _Size, size_type s0, size_type s1, size_type s2, size_type s3>
    struct swizzle<bool, _Size, s0, s1, s2, s3>
    {
        using scalar = bool;
        static constexpr size_type size = 4;
        using type = swizzle<bool, _Size, s0, s1, s2, s3>;
        using conv_type = vector<scalar, 4>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y, scalar z, scalar w) noexcept { data[s0] = x; data[s1] = y; data[s2] = z; data[s3] = w; }

        constexpr void set_mask(bitfield8 mask) noexcept { data[s0] = mask & 1; data[s1] = mask & 2; data[s2] = mask & 4; data[s3] = mask & 8; }
        constexpr bitfield8 mask() const noexcept { return static_cast<bitfield8>(data[s0] | data[s1] << 1u | data[s2] << 2u | data[s3] << 3u); }

        constexpr operator conv_type() const noexcept
        {
            conv_type result;
            result.data[0] = data[s0];
            result.data[1] = data[s1];
            result.data[2] = data[s2];
            result.data[3] = data[s3];
            return result;
        }

        constexpr swizzle& operator=(const conv_type& other) noexcept
        {
            data[s0] = other[0]; data[s1] = other[1]; data[s2] = other[2]; data[s3] = other[3];
            return *this;
        }
    };
}
