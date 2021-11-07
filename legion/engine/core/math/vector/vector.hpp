#pragma once
#include <core/common/assert.hpp>
#include <core/types/primitives.hpp>

namespace legion::core::math
{
    template<typename _Scalar, size_type _Size>
    struct vector;

    template<typename _Scalar, size_type _Size>
    vector<_Scalar, _Size> sqrt(const vector<_Scalar, _Size>& v) noexcept;

    template<typename _Scalar, size_type _Size>
    constexpr _Scalar dot(const vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b) noexcept;

    template<typename _Scalar, size_type _Size>
    constexpr vector<_Scalar, _Size> cross(const vector<_Scalar, _Size>& a, const vector<_Scalar, _Size>& b) noexcept;


    template<typename _Scalar, size_type _Size>
    struct vector
    {
        static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

        using scalar = _Scalar;
        static constexpr size_type size = _Size;
        using type = vector<_Scalar, _Size>;

        union
        {
            struct
            {
                scalar x, y, z, w;
            };
            scalar data[size];
        };

        constexpr vector() noexcept { for (size_type i = 0; i < size; i++) data[i] = static_cast<scalar>(0); }
        constexpr vector(const vector&) noexcept = default;
        explicit constexpr vector(scalar s) noexcept { for (size_type i = 0; i < size; i++) data[i] = s; }

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
        {
            for (size_type i = 0; i < size; i++) data[i] = static_cast<scalar>(other.data[i]);
        }

        L_ALWAYS_INLINE scalar length() const noexcept { return sqrt(dot(*this, *this)); }
        constexpr scalar length2() const noexcept { return dot(*this, *this); }
    };
}
