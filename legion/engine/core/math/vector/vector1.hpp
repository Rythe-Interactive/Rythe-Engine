#pragma once
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    template<typename _Scalar>
    struct alignas(sizeof(_Scalar) * 1) vector<_Scalar, 1>
    {
        static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

        using scalar = _Scalar;
        static constexpr size_type size = 1;
        using type = vector<_Scalar, 1>;

        union
        {
            scalar x;
            scalar data[size];
        };

        constexpr vector() noexcept : x(static_cast<scalar>(0)) {}
        constexpr vector(const vector&) noexcept = default;
        constexpr vector(scalar s) noexcept : x(static_cast<scalar>(s)) {}

        static constexpr vector one = vector(static_cast<scalar>(1));
        static constexpr vector zero = vector(static_cast<scalar>(0));

        constexpr vector& operator=(const vector&) noexcept = default;
        constexpr operator scalar() noexcept { return x; }

        constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
        constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }

        template<typename _Scal>
        constexpr explicit vector(const vector<_Scal, size>& other) noexcept : x(static_cast<scalar>(other.x)) {}

        L_ALWAYS_INLINE scalar length() const noexcept { return sqrt(dot(*this, *this)); }
        constexpr scalar length2() const noexcept { return dot(*this, *this); }
    };

    using vec1 = vector<float, 1>;
    using dvec1 = vector<double, 1>;
    using ivec1 = vector<int, 1>;
    using bvec1 = vector<bool, 1>;
}
