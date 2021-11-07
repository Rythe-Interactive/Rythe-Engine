#pragma once
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    template<typename _Scalar>
    struct alignas(sizeof(_Scalar) * 2) vector<_Scalar, 2>
    {
        static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

        using scalar = _Scalar;
        static constexpr size_type size = 2;
        using type = vector<_Scalar, 2>;

        union
        {
            struct
            {
                scalar x, y;
            };
            scalar data[2];
        };

        constexpr vector() noexcept : x(static_cast<scalar>(0)), y(static_cast<scalar>(0)) {}
        constexpr vector(const vector&) noexcept = default;
        explicit constexpr vector(scalar s) noexcept : x(static_cast<scalar>(s)), y(static_cast<scalar>(s)) {}
        constexpr vector(scalar _x, scalar _y) noexcept : x(_x), y(_y) {}

        static constexpr vector up = vector(static_cast<scalar>(0), static_cast<scalar>(1));
        static constexpr vector down = vector(static_cast<scalar>(0), static_cast<scalar>(-1));
        static constexpr vector right = vector(static_cast<scalar>(1), static_cast<scalar>(0));
        static constexpr vector left = vector(static_cast<scalar>(-1), static_cast<scalar>(0));
        static constexpr vector one = vector(static_cast<scalar>(1));
        static constexpr vector zero = vector(static_cast<scalar>(0));

        constexpr vector& operator=(const vector&) noexcept = default;

        constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
        constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }

        template<typename _Scal>
        constexpr explicit vector(const vector<_Scal, size>& other) noexcept : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)) {}

        L_ALWAYS_INLINE scalar length() const noexcept { return sqrt(dot(*this, *this)); }
        constexpr scalar length2() const noexcept { return dot(*this, *this); }
        L_ALWAYS_INLINE scalar angle() const noexcept { return std::atan2(y, x); }
    };

    using vec2 = vector<float, 2>;
    using dvec2 = vector<double, 2>;
    using ivec2 = vector<int, 2>;
    using bvec2 = vector<bool, 2>;
}
