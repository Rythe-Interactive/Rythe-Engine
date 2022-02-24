#pragma once
#include <tuple>

#include <core/common/assert.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>

namespace legion::core::math
{
    template<typename Scalar, size_type Size>
    struct vector
    {
        static_assert(::std::is_arithmetic_v<Scalar>, "Scalar must be a numeric type.");

        using scalar = Scalar;
        static constexpr size_type size = Size;
        using type = vector<Scalar, Size>;

        scalar data[size];

        static const vector one;
        static const vector zero;

        constexpr vector() noexcept;
        constexpr vector(const vector&) noexcept = default;
        explicit constexpr vector(scalar s) noexcept;

        template<typename Scal, ::std::enable_if_t<!::std::is_same_v<scalar, Scal>, bool> = true>
        constexpr explicit vector(const vector<Scal, size>& other) noexcept;

        template<typename vec_type, ::std::enable_if_t<Size != vec_type::size, bool> = true>
        constexpr vector(const vec_type& other) noexcept;

        constexpr vector& operator=(const vector&) noexcept = default;

        L_NODISCARD constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL);
        L_NODISCARD constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL);

        L_NODISCARD L_ALWAYS_INLINE scalar length() const noexcept;
        L_NODISCARD constexpr scalar length2() const noexcept;
    };
}
