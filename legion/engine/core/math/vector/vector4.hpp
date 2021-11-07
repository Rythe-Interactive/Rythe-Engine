#pragma once
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
        };

        constexpr vector() noexcept
            : x(static_cast<scalar>(0)), y(static_cast<scalar>(0)), z(static_cast<scalar>(0)), w(static_cast<scalar>(0)) {}
        constexpr vector(const vector&) noexcept = default;
        explicit constexpr vector(scalar s) noexcept
            : x(static_cast<scalar>(s)), y(static_cast<scalar>(s)), z(static_cast<scalar>(s)), w(static_cast<scalar>(s)) {}
        constexpr vector(scalar _x, scalar _y, scalar _z, scalar _w) noexcept : x(_x), y(_y), z(_z), w(_w) {}

        static constexpr vector up = vector(static_cast<scalar>(0), static_cast<scalar>(1), static_cast<scalar>(0), static_cast<scalar>(0));
        static constexpr vector down = vector(static_cast<scalar>(0), static_cast<scalar>(-1), static_cast<scalar>(0), static_cast<scalar>(0));
        static constexpr vector right = vector(static_cast<scalar>(1), static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0));
        static constexpr vector left = vector(static_cast<scalar>(-1), static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0));
        static constexpr vector forward = vector(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(1), static_cast<scalar>(0));
        static constexpr vector backward = vector(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(-1), static_cast<scalar>(0));
        static constexpr vector positiveW = vector(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(1));
        static constexpr vector negativeW = vector(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(-1));
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
        constexpr explicit vector(const vector<_Scal, size>& other) noexcept
            : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)),
            z(static_cast<scalar>(other.z)), w(static_cast<scalar>(other.w)) {}

        L_ALWAYS_INLINE scalar length() const noexcept { return sqrt(dot(*this, *this)); }
        constexpr scalar length2() const noexcept { return dot(*this, *this); }
    };

    using vec4 = vector<float, 4>;
    using dvec4 = vector<double, 4>;
    using ivec4 = vector<int, 4>;
    using bvec4 = vector<bool, 4>;
}
