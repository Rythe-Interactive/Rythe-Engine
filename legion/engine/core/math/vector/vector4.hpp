#pragma once
#include <immintrin.h>

#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    template<typename _Scalar>
    struct alignas(sizeof(_Scalar) * 4) vector<_Scalar, 4>
    {
        static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

        using scalar = _Scalar;
        static constexpr size_type size = 4;
        using type = vector<_Scalar, 4>;

        union
        {
            struct
            {
                scalar x, y, z, w;
            };
            scalar data[4];
            __m128 intrin[(sizeof(_Scalar) * 4 * 8) / 128];
        };

        constexpr vector() noexcept
            : x(static_cast<scalar>(0)), y(static_cast<scalar>(0)), z(static_cast<scalar>(0)), w(static_cast<scalar>(0)) {}
        constexpr vector(const vector&) noexcept = default;
        explicit constexpr vector(scalar s) noexcept
            : x(static_cast<scalar>(s)), y(static_cast<scalar>(s)), z(static_cast<scalar>(s)), w(static_cast<scalar>(s)) {}
        constexpr vector(scalar _x, scalar _y, scalar _z, scalar _w) noexcept : x(_x), y(_y), z(_z), w(_w) {}

        static const vector up;
        static const vector down;
        static const vector right;
        static const vector left;
        static const vector forward;
        static const vector backward;
        static const vector positiveW;
        static const vector negativeW;
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
        constexpr explicit vector(const vector<_Scal, size>& other) noexcept
            : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)),
            z(static_cast<scalar>(other.z)), w(static_cast<scalar>(other.w)) {}

        L_ALWAYS_INLINE scalar length() const noexcept { return ::legion::core::math::length(*this); }
        constexpr scalar length2() const noexcept { return ::legion::core::math::length2(*this); }
    };

    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::up(static_cast<_Scalar>(0), static_cast<_Scalar>(1), static_cast<_Scalar>(0), static_cast<scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::down(static_cast<_Scalar>(0), static_cast<_Scalar>(-1), static_cast<_Scalar>(0), static_cast<scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::right(static_cast<_Scalar>(1), static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::left(static_cast<_Scalar>(-1), static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::forward(static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<_Scalar>(1), static_cast<scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::backward(static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<_Scalar>(-1), static_cast<scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::positiveW(static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<scalar>(0), static_cast<_Scalar>(1));
    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::negativeW(static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<scalar>(0), static_cast<_Scalar>(-1));
    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::one(static_cast<_Scalar>(1));
    template<typename _Scalar>
    const vector<_Scalar, 4> vector<_Scalar, 4>::zero(static_cast<_Scalar>(0));

    using float4 = vector<float, 4>;
    using double4 = vector<double, 4>;
    using int4 = vector<int, 4>;
    using bool4 = vector<bool, 4>;
}
