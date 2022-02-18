#pragma once
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/math/vector/vector_base.hpp>

namespace legion::core::math
{
    template<typename Scalar, size_type Size, size_type... args>
    struct swizzle;

    template<typename Scalar, size_type Size, size_type s0>
    struct swizzle<Scalar, Size, s0>
    {
        using scalar = Scalar;
        static constexpr size_type size = 1;
        using type = swizzle<Scalar, Size, s0>;
        using conv_type = vector<scalar, 1>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x) noexcept;

        constexpr operator scalar() const noexcept;

        constexpr operator conv_type() const noexcept;

        constexpr swizzle& operator=(const conv_type& other) noexcept;

        constexpr swizzle& operator=(scalar value) noexcept;
    };

    template<size_type Size, size_type s0>
    struct swizzle<bool, Size, s0>
    {
        using scalar = bool;
        static constexpr size_type size = 1;
        using type = swizzle<bool, Size, s0>;
        using conv_type = vector<scalar, 1>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x) noexcept;

        constexpr void set_mask(bitfield8 mask) noexcept;
        constexpr bitfield8 mask() const noexcept;

        constexpr operator scalar() const noexcept;

        constexpr operator conv_type() const noexcept;

        constexpr swizzle& operator=(const conv_type& other) noexcept;

        constexpr swizzle& operator=(scalar value) noexcept;
    };

    template<typename Scalar, size_type Size, size_type s0, size_type s1>
    struct swizzle<Scalar, Size, s0, s1>
    {
        using scalar = Scalar;
        static constexpr size_type size = 2;
        using type = swizzle<Scalar, Size, s0, s1>;
        using conv_type = vector<scalar, 2>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y) noexcept;

        constexpr operator conv_type() const noexcept;

        constexpr swizzle& operator=(const conv_type& other) noexcept;
    };

    template<size_type Size, size_type s0, size_type s1>
    struct swizzle<bool, Size, s0, s1>
    {
        using scalar = bool;
        static constexpr size_type size = 2;
        using type = swizzle<bool, Size, s0, s1>;
        using conv_type = vector<scalar, 2>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y) noexcept;

        constexpr void set_mask(bitfield8 mask) noexcept;
        constexpr bitfield8 mask() const noexcept;

        constexpr operator conv_type() const noexcept;

        constexpr swizzle& operator=(const conv_type& other) noexcept;
    };

    template<typename Scalar, size_type Size, size_type s0, size_type s1, size_type s2>
    struct swizzle<Scalar, Size, s0, s1, s2>
    {
        using scalar = Scalar;
        static constexpr size_type size = 3;
        using type = swizzle<Scalar, Size, s0, s1, s2>;
        using conv_type = vector<scalar, 3>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y, scalar z) noexcept;

        constexpr operator conv_type() const noexcept;

        constexpr swizzle& operator=(const conv_type& other) noexcept;
    };

    template<size_type Size, size_type s0, size_type s1, size_type s2>
    struct swizzle<bool, Size, s0, s1, s2>
    {
        using scalar = bool;
        static constexpr size_type size = 3;
        using type = swizzle<bool, Size, s0, s1, s2>;
        using conv_type = vector<scalar, 3>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y, scalar z) noexcept;

        constexpr void set_mask(bitfield8 mask) noexcept;
        constexpr bitfield8 mask() const noexcept;

        constexpr operator conv_type() const noexcept;

        constexpr swizzle& operator=(const conv_type& other) noexcept;
    };

    template<typename Scalar, size_type Size, size_type s0, size_type s1, size_type s2, size_type s3>
    struct swizzle<Scalar, Size, s0, s1, s2, s3>
    {
        using scalar = Scalar;
        static constexpr size_type size = 4;
        using type = swizzle<Scalar, Size, s0, s1, s2, s3>;
        using conv_type = vector<scalar, 4>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y, scalar z, scalar w) noexcept;

        constexpr operator conv_type() const noexcept;

        constexpr swizzle& operator=(const conv_type& other) noexcept;
    };

    template<size_type Size, size_type s0, size_type s1, size_type s2, size_type s3>
    struct swizzle<bool, Size, s0, s1, s2, s3>
    {
        using scalar = bool;
        static constexpr size_type size = 4;
        using type = swizzle<bool, Size, s0, s1, s2, s3>;
        using conv_type = vector<scalar, 4>;

        scalar data[size];

        RULE_OF_5_CONSTEXPR_NOEXCEPT(swizzle);

        constexpr swizzle(scalar x, scalar y, scalar z, scalar w) noexcept;

        constexpr void set_mask(bitfield8 mask) noexcept;
        constexpr bitfield8 mask() const noexcept;

        constexpr operator conv_type() const noexcept;

        constexpr swizzle& operator=(const conv_type& other) noexcept;
    };
}
