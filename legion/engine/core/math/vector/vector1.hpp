#pragma once
#include <core/math/vector/vector_base.hpp>
#include <core/math/vector/swizzle/swizzle1.hpp>

namespace legion::core::math
{
    template<typename Scalar>
    struct alignas(sizeof(Scalar) * 1) vector<Scalar, 1>
    {
        static_assert(::std::is_arithmetic_v<Scalar>, "Scalar must be a numeric type.");

        using scalar = Scalar;
        static constexpr size_type size = 1;
        using type = vector<Scalar, 1>;

        union
        {
            scalar data[size];

            _MATH_SWIZZLE_1_1_(scalar);
            _MATH_SWIZZLE_1_2_(scalar);
            _MATH_SWIZZLE_1_3_(scalar);
            _MATH_SWIZZLE_1_4_(scalar);
        };

        constexpr vector() noexcept;
        constexpr vector(const vector&) noexcept = default;
        constexpr vector(scalar s) noexcept;

        static const vector one;
        static const vector zero;

        constexpr vector& operator=(const vector&) noexcept = default;
        constexpr operator scalar() const noexcept;

        constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL);
        constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL);

        constexpr scalar length() const noexcept;
        constexpr scalar length2() const noexcept;
    };

    template<>
    struct alignas(sizeof(bool) * 1) vector<bool, 1>
    {
        using scalar = bool;
        static constexpr size_type size = 1;
        using type = vector<bool, 1>;

        union
        {
            scalar data[size];

            _MATH_SWIZZLE_1_1_(scalar);
            _MATH_SWIZZLE_1_2_(scalar);
            _MATH_SWIZZLE_1_3_(scalar);
            _MATH_SWIZZLE_1_4_(scalar);
        };

        constexpr vector() noexcept : x(static_cast<scalar>(0)) {}
        constexpr vector(const vector&) noexcept = default;
        constexpr vector(scalar s) noexcept : x(static_cast<scalar>(s)) {}

        static const vector one;
        static const vector zero;

        constexpr void set_mask(bitfield8 mask) noexcept { x = mask & 1; }
        constexpr bitfield8 mask() const noexcept { return static_cast<bitfield8>(x); }

        constexpr vector& operator=(const vector&) noexcept = default;
        constexpr operator scalar() const noexcept { return x; }

        constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
        constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }

        constexpr scalar length() const noexcept { return x; }
        constexpr scalar length2() const noexcept { return x * x; }
    };

    using float1 = vector<float32, 1>;
    using double1 = vector<float64, 1>;
    using int1 = vector<int, 1>;
    using uint1 = vector<uint, 1>;
    using bool1 = vector<bool, 1>;
}

#include <core/math/vector/vector1.inl>
