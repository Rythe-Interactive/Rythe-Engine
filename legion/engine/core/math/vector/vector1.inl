#include <core/math/vector/vector1.hpp>
#pragma once

namespace legion::core::math
{
    template<typename scalar>
    constexpr vector<scalar, 1>::vector() noexcept : x(static_cast<scalar>(0)) {}
    template<typename scalar>
    constexpr vector<scalar, 1>::vector(scalar s) noexcept : x(s) {}

    template<typename scalar>
    const vector<scalar, 1> one = vector<scalar, 1>(static_cast<scalar>(1));
    template<typename scalar>
    const vector<scalar, 1> zero = vector<scalar, 1>(static_cast<scalar>(0));

    template<typename scalar>
    constexpr vector<scalar, 1>::operator scalar() const noexcept { return x; }

    template<typename scalar>
    constexpr scalar& vector<scalar, 1>::operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
    {
        assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
    }

    template<typename scalar>
    constexpr const scalar& vector<scalar, 1>::operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
    {
        assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
    }

    template<typename scalar>
    constexpr scalar vector<scalar, 1>::length() const noexcept { return x; }
    template<typename scalar>
    constexpr scalar vector<scalar, 1>::length2() const noexcept { return x * x; }
}
