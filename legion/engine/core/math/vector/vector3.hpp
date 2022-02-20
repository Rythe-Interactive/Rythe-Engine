#pragma once
#include <core/math/vector/vector_base.hpp>
#include <core/math/vector/swizzle/swizzle3.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    template<typename Scalar>
    struct vector<Scalar, 3>
    {
        static_assert(std::is_arithmetic<Scalar>::value, "Scalar must be a numeric type.");

        using scalar = Scalar;
        static constexpr size_type size = 3;
        using type = vector<Scalar, 3>;

        union
        {
            scalar data[3];

            _MATH_SWIZZLE_3_1_(scalar);
            _MATH_SWIZZLE_3_2_(scalar);
            _MATH_SWIZZLE_3_3_(scalar);
            _MATH_SWIZZLE_3_4_(scalar);
        };

        constexpr vector() noexcept : xyz(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0)) {}

        constexpr vector(const vector&) noexcept = default;

        explicit constexpr vector(scalar s) noexcept : xyz(static_cast<scalar>(s), static_cast<scalar>(s), static_cast<scalar>(s)) {}

        constexpr vector(scalar _x, scalar _y, scalar _z) noexcept : xyz(_x, _y, _z) {}

        template<typename vec_type, ::std::enable_if_t<is_vector_v<vec_type> && (size != vec_type::size || !std::is_same_v<scalar, typename vec_type::scalar>), bool> = true>
        constexpr vector(const vec_type& other) noexcept
        {
            if constexpr (size > vec_type::size)
            {
                for (size_type i = 0; i < vec_type::size; i++)
                    data[i] = static_cast<scalar>(other.data[i]);

                for (size_type i = vec_type::size; i < size; i++)
                    data[i] = static_cast<scalar>(0);
            }
            else
            {
                for (size_type i = 0; i < size; i++)
                    data[i] = static_cast<scalar>(other.data[i]);
            }
        }

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

        L_ALWAYS_INLINE scalar length() const noexcept { return ::legion::core::math::length(*this); }
        constexpr scalar length2() const noexcept { return ::legion::core::math::length2(*this); }
    };

    template<>
    struct vector<bool, 3>
    {
        using scalar = bool;
        static constexpr size_type size = 3;
        using type = vector<bool, 3>;

        union
        {
            scalar data[3];

            _MATH_SWIZZLE_3_1_(scalar);
            _MATH_SWIZZLE_3_2_(scalar);
            _MATH_SWIZZLE_3_3_(scalar);
            _MATH_SWIZZLE_3_4_(scalar);
        };

        constexpr vector() noexcept : xyz(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0)) {}

        constexpr vector(const vector&) noexcept = default;

        explicit constexpr vector(scalar s) noexcept : xyz(static_cast<scalar>(s), static_cast<scalar>(s), static_cast<scalar>(s)) {}

        constexpr vector(scalar _x, scalar _y, scalar _z) noexcept : xyz(_x, _y, _z) {}

        template<typename _Scal, ::std::enable_if_t<!::std::is_same_v<scalar, _Scal>, bool> = true>
        constexpr vector(const vector<_Scal, size>& other) noexcept
            : xyz(static_cast<scalar>(other.x), static_cast<scalar>(other.y), static_cast<scalar>(other.z)) {}

        template<typename vec_type, ::std::enable_if_t<is_vector_v<vec_type> && (size != vec_type::size), bool> = true>
        constexpr vector(const vec_type& other) noexcept
        {
            if constexpr (size > vec_type::size)
            {
                for (size_type i = 0; i < vec_type::size; i++)
                    data[i] = static_cast<scalar>(other.data[i]);

                for (size_type i = vec_type::size; i < size; i++)
                    data[i] = static_cast<scalar>(0);
            }
            else
            {
                for (size_type i = 0; i < size; i++)
                    data[i] = static_cast<scalar>(other.data[i]);
            }
        }

        static const vector up;
        static const vector down;
        static const vector right;
        static const vector left;
        static const vector forward;
        static const vector backward;
        static const vector one;
        static const vector zero;

        constexpr void set_mask(bitfield8 mask) noexcept { x = mask & 1; y = mask & 2; z = mask & 4; }
        constexpr bitfield8 mask() const noexcept { return static_cast<bitfield8>(x | y << 1u | z << 2u); }

        constexpr operator bool() const noexcept { return x && y && z; }

        constexpr vector& operator=(const vector&) noexcept = default;

        constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
        constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }

        constexpr scalar length() const noexcept { return x || y || z; }
        constexpr scalar length2() const noexcept { return this->length(); }
    };

    template<typename Scalar>
    const vector<Scalar, 3> vector<Scalar, 3>::up(static_cast<Scalar>(0), static_cast<Scalar>(1), static_cast<Scalar>(0));
    template<typename Scalar>
    const vector<Scalar, 3> vector<Scalar, 3>::down(static_cast<Scalar>(0), static_cast<Scalar>(-1), static_cast<Scalar>(0));
    template<typename Scalar>
    const vector<Scalar, 3> vector<Scalar, 3>::right(static_cast<Scalar>(1), static_cast<Scalar>(0), static_cast<Scalar>(0));
    template<typename Scalar>
    const vector<Scalar, 3> vector<Scalar, 3>::left(static_cast<Scalar>(-1), static_cast<Scalar>(0), static_cast<Scalar>(0));
    template<typename Scalar>
    const vector<Scalar, 3> vector<Scalar, 3>::forward(static_cast<Scalar>(0), static_cast<Scalar>(0), static_cast<Scalar>(1));
    template<typename Scalar>
    const vector<Scalar, 3> vector<Scalar, 3>::backward(static_cast<Scalar>(0), static_cast<Scalar>(0), static_cast<Scalar>(-1));
    template<typename Scalar>
    const vector<Scalar, 3> vector<Scalar, 3>::one(static_cast<Scalar>(1));
    template<typename Scalar>
    const vector<Scalar, 3> vector<Scalar, 3>::zero(static_cast<Scalar>(0));

    using float3 = vector<float32, 3>;
    using double3 = vector<float64, 3>;
    using int3 = vector<int, 3>;
    using bool3 = vector<bool, 3>;
}
