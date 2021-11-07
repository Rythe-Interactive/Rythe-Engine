#pragma once
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    template<typename _Scalar>
    struct vector<_Scalar, 3>
    {
        static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

        using scalar = _Scalar;
        static constexpr size_type size = 3;
        using type = vector<_Scalar, 3>;

        union
        {
            struct
            {
                scalar x, y, z;
            };
            scalar data[3];
        };

        constexpr vector() noexcept : x(static_cast<scalar>(0)), y(static_cast<scalar>(0)), z(static_cast<scalar>(0)) {}
        constexpr vector(const vector&) noexcept = default;
        explicit constexpr vector(scalar s) noexcept : x(static_cast<scalar>(s)), y(static_cast<scalar>(s)), z(static_cast<scalar>(s)) {}
        constexpr vector(scalar _x, scalar _y, scalar _z) noexcept : x(_x), y(_y), z(_z) {}

        static constexpr vector up = vector(static_cast<scalar>(0), static_cast<scalar>(1), static_cast<scalar>(0));
        static constexpr vector down = vector(static_cast<scalar>(0), static_cast<scalar>(-1), static_cast<scalar>(0));
        static constexpr vector right = vector(static_cast<scalar>(1), static_cast<scalar>(0), static_cast<scalar>(0));
        static constexpr vector left = vector(static_cast<scalar>(-1), static_cast<scalar>(0), static_cast<scalar>(0));
        static constexpr vector forward = vector(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(1));
        static constexpr vector backward = vector(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(-1));
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
            : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)), z(static_cast<scalar>(other.z)) {}

        L_ALWAYS_INLINE scalar length() const noexcept { return sqrt(dot(*this, *this)); }
        constexpr scalar length2() const noexcept { return dot(*this, *this); }
    };

    template<typename _Scalar>
    struct alignas(sizeof(_Scalar) * 4) aligned_vector3 : public vector<float, 3>
    {
        static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

        using scalar = _Scalar;
        static constexpr size_type size = 3;
        using type = aligned_vector3<_Scalar>;

        using vector<float, 3>::vector;
        using vector<float, 3>::operator=;
        using vector<float, 3>::operator[];

        static constexpr aligned_vector3 up = aligned_vector3(static_cast<scalar>(0), static_cast<scalar>(1), static_cast<scalar>(0));
        static constexpr aligned_vector3 down = aligned_vector3(static_cast<scalar>(0), static_cast<scalar>(-1), static_cast<scalar>(0));
        static constexpr aligned_vector3 right = aligned_vector3(static_cast<scalar>(1), static_cast<scalar>(0), static_cast<scalar>(0));
        static constexpr aligned_vector3 left = aligned_vector3(static_cast<scalar>(-1), static_cast<scalar>(0), static_cast<scalar>(0));
        static constexpr aligned_vector3 forward = aligned_vector3(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(1));
        static constexpr aligned_vector3 backward = aligned_vector3(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(-1));
        static constexpr aligned_vector3 one = aligned_vector3(static_cast<scalar>(1));
        static constexpr aligned_vector3 zero = aligned_vector3(static_cast<scalar>(0));
    };

    using vec3 = vector<float, 3>;
    using dvec3 = vector<double, 3>;
    using ivec3 = vector<int, 3>;
    using bvec3 = vector<bool, 3>;
    using aligned_vec3 = aligned_vector3<float>;
    using aligned_dvec3 = aligned_vector3<double>;
    using aligned_ivec3 = aligned_vector3<int>;
    using aligned_bvec3 = aligned_vector3<bool>;
}
