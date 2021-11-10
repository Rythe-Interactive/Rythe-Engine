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

        static const vector up;
        static const vector down;
        static const vector right;
        static const vector left;
        static const vector one;
        static const vector zero;

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

        L_ALWAYS_INLINE scalar length() const noexcept { return ::legion::core::math::length(*this); }
        constexpr scalar length2() const noexcept { return ::legion::core::math::length2(*this); }
        L_ALWAYS_INLINE scalar angle() const noexcept { return ::std::atan2(y, x); }
    };

    template<typename _Scalar>
    const vector<_Scalar, 2> vector<_Scalar, 2>::up(static_cast<_Scalar>(0), static_cast<_Scalar>(1));
    template<typename _Scalar>
    const vector<_Scalar, 2> vector<_Scalar, 2>::down(static_cast<_Scalar>(0), static_cast<_Scalar>(-1));
    template<typename _Scalar>
    const vector<_Scalar, 2> vector<_Scalar, 2>::right(static_cast<_Scalar>(1), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 2> vector<_Scalar, 2>::left(static_cast<_Scalar>(-1), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 2> vector<_Scalar, 2>::one(static_cast<_Scalar>(1));
    template<typename _Scalar>
    const vector<_Scalar, 2> vector<_Scalar, 2>::zero(static_cast<_Scalar>(0));

    using float2 = vector<float, 2>;
    using double2 = vector<double, 2>;
    using int2 = vector<int, 2>;
    using bool2 = vector<bool, 2>;
}
