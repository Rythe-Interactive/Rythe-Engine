#pragma once
#include <core/math/vector/vector_base.hpp>

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

        static const vector up;
        static const vector down;
        static const vector right;
        static const vector left;
        static const vector forward;
        static const vector backward;
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
            : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)), z(static_cast<scalar>(other.z)) {}

        L_ALWAYS_INLINE scalar length() const noexcept { return ::legion::core::math::length(*this); }
        constexpr scalar length2() const noexcept { return ::legion::core::math::length2(*this); }
    };

    template<typename _Scalar>
    const vector<_Scalar, 3> vector<_Scalar, 3>::up(static_cast<_Scalar>(0), static_cast<_Scalar>(1), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 3> vector<_Scalar, 3>::down(static_cast<_Scalar>(0), static_cast<_Scalar>(-1), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 3> vector<_Scalar, 3>::right(static_cast<_Scalar>(1), static_cast<_Scalar>(0), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 3> vector<_Scalar, 3>::left(static_cast<_Scalar>(-1), static_cast<_Scalar>(0), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const vector<_Scalar, 3> vector<_Scalar, 3>::forward(static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<_Scalar>(1));
    template<typename _Scalar>
    const vector<_Scalar, 3> vector<_Scalar, 3>::backward(static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<_Scalar>(-1));
    template<typename _Scalar>
    const vector<_Scalar, 3> vector<_Scalar, 3>::one(static_cast<_Scalar>(1));
    template<typename _Scalar>
    const vector<_Scalar, 3> vector<_Scalar, 3>::zero(static_cast<_Scalar>(0));

    template<typename _Scalar>
    struct alignas(sizeof(_Scalar) * 4) aligned_vector3 : public vector<float, 3>
    {
        static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

        using scalar = _Scalar;
        static constexpr size_type size = 3;
        using type = aligned_vector3<_Scalar>;

        byte padding[sizeof(_Scalar)];

        using vector<float, 3>::vector;
        using vector<float, 3>::operator=;
        using vector<float, 3>::operator[];

        static const aligned_vector3 up;
        static const aligned_vector3 down;
        static const aligned_vector3 right;
        static const aligned_vector3 left;
        static const aligned_vector3 forward;
        static const aligned_vector3 backward;
        static const aligned_vector3 one;
        static const aligned_vector3 zero;
    };

    template<typename _Scalar>
    const aligned_vector3<_Scalar> aligned_vector3<_Scalar>::up(static_cast<_Scalar>(0), static_cast<_Scalar>(1), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const aligned_vector3<_Scalar> aligned_vector3<_Scalar>::down(static_cast<_Scalar>(0), static_cast<_Scalar>(-1), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const aligned_vector3<_Scalar> aligned_vector3<_Scalar>::right(static_cast<_Scalar>(1), static_cast<_Scalar>(0), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const aligned_vector3<_Scalar> aligned_vector3<_Scalar>::left(static_cast<_Scalar>(-1), static_cast<_Scalar>(0), static_cast<_Scalar>(0));
    template<typename _Scalar>
    const aligned_vector3<_Scalar> aligned_vector3<_Scalar>::forward(static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<_Scalar>(1));
    template<typename _Scalar>
    const aligned_vector3<_Scalar> aligned_vector3<_Scalar>::backward(static_cast<_Scalar>(0), static_cast<_Scalar>(0), static_cast<_Scalar>(-1));
    template<typename _Scalar>
    const aligned_vector3<_Scalar> aligned_vector3<_Scalar>::one(static_cast<_Scalar>(1));
    template<typename _Scalar>
    const aligned_vector3<_Scalar> aligned_vector3<_Scalar>::zero(static_cast<_Scalar>(0));

    using float3 = vector<float, 3>;
    using double3 = vector<double, 3>;
    using int3 = vector<int, 3>;
    using bool3 = vector<bool, 3>;
    using aligned_float3 = aligned_vector3<float>;
    using aligned_double3 = aligned_vector3<double>;
    using aligned_int3 = aligned_vector3<int>;
    using aligned_bool3 = aligned_vector3<bool>;
}
