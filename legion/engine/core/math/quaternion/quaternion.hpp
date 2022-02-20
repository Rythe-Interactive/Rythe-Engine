#pragma once
#include <immintrin.h>

#include <core/common/assert.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>

#include <core/math/meta.hpp>

namespace legion::core::math
{
    template<typename Scalar>
    struct quaternion
    {
        static_assert(std::is_arithmetic<Scalar>::value, "Scalar must be a numeric type.");

        using scalar = Scalar;
        static constexpr size_type size = 4;
        using type = quaternion<Scalar>;

        union
        {
            struct
            {
                scalar w, i, j, k;
            };
            __m128 intrin[(sizeof(scalar) * size) / sizeof(__m128)];

            scalar data[size];
        };

        constexpr quaternion() noexcept
            : w(static_cast<scalar>(1)), i(static_cast<scalar>(0)), j(static_cast<scalar>(0)), k(static_cast<scalar>(0)) {}

        constexpr quaternion(const quaternion&) noexcept = default;

        constexpr quaternion(scalar _w, scalar _i, scalar _j, scalar _k) noexcept
            : w(static_cast<scalar>(_w)), i(static_cast<scalar>(_i)), j(static_cast<scalar>(_j)), k(static_cast<scalar>(_k)) {}

        template<typename vec_type, ::std::enable_if_t<is_vector_v<vec_type>&& vec_type::size == 3, bool> = true>
        constexpr quaternion(typename vec_type::scalar s, const vec_type& v) noexcept
            : w(static_cast<scalar>(s)), i(static_cast<scalar>(v.x)), j(static_cast<scalar>(v.y)), k(static_cast<scalar>(v.z)) {}

        template<typename vec_type, ::std::enable_if_t<is_vector_v<vec_type>&& vec_type::size == 4, bool> = true>
        explicit constexpr quaternion(const vec_type& other) noexcept
            : w(static_cast<scalar>(other.w)), i(static_cast<scalar>(other.x)), j(static_cast<scalar>(other.y)), k(static_cast<scalar>(other.z)) {}

        static const quaternion identity;
        static const quaternion rotate_x_90;
        static const quaternion rotate_y_90;
        static const quaternion rotate_z_90;
        static const quaternion rotate_x_180;
        static const quaternion rotate_y_180;
        static const quaternion rotate_z_180;

        constexpr quaternion& operator=(const quaternion&) noexcept = default;

        constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("quaternion subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
        constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("quaternion subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
    };

#define sin45 0.70710678118654752440084436210485L
#define cos45 0.70710678118654752440084436210485L

    template<typename Scalar>
    const quaternion<Scalar> quaternion<Scalar>::identity(static_cast<Scalar>(1), static_cast<Scalar>(0), static_cast<Scalar>(0), static_cast<Scalar>(0));
    template<typename Scalar>
    const quaternion<Scalar> quaternion<Scalar>::rotate_x_90(static_cast<Scalar>(cos45), static_cast<Scalar>(sin45), static_cast<Scalar>(0), static_cast<Scalar>(0));
    template<typename Scalar>
    const quaternion<Scalar> quaternion<Scalar>::rotate_y_90(static_cast<Scalar>(cos45), static_cast<Scalar>(0), static_cast<Scalar>(sin45), static_cast<Scalar>(0));
    template<typename Scalar>
    const quaternion<Scalar> quaternion<Scalar>::rotate_z_90(static_cast<Scalar>(cos45), static_cast<Scalar>(0), static_cast<Scalar>(0), static_cast<Scalar>(sin45));
    template<typename Scalar>
    const quaternion<Scalar> quaternion<Scalar>::rotate_x_180(static_cast<Scalar>(0), static_cast<Scalar>(1), static_cast<Scalar>(0), static_cast<Scalar>(0));
    template<typename Scalar>
    const quaternion<Scalar> quaternion<Scalar>::rotate_y_180(static_cast<Scalar>(0), static_cast<Scalar>(0), static_cast<Scalar>(1), static_cast<Scalar>(0));
    template<typename Scalar>
    const quaternion<Scalar> quaternion<Scalar>::rotate_z_180(static_cast<Scalar>(0), static_cast<Scalar>(0), static_cast<Scalar>(0), static_cast<Scalar>(1));

#undef sin45
#undef cos45

    using quat = quaternion<float32>;
    using quat64 = quaternion<float64>;
    using quat_max = quaternion<float_max>;
}
