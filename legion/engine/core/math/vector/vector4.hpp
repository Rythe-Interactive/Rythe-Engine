#pragma once
#include <immintrin.h>

#include <core/math/vector/vector_base.hpp>
#include <core/math/vector/swizzle/swizzle4.hpp>
#include <core/math/meta.hpp>

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
            scalar data[4];
            __m128 intrin[(sizeof(scalar) * size) / sizeof(__m128)];

            _MATH_SWIZZLE_4_1_(scalar);
            _MATH_SWIZZLE_4_2_(scalar);
            _MATH_SWIZZLE_4_3_(scalar);
            _MATH_SWIZZLE_4_4_(scalar);
        };

        constexpr vector() noexcept
            : xyzw(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0)) {}

        constexpr vector(const vector&) noexcept = default;

        explicit constexpr vector(scalar s) noexcept
            : xyzw(static_cast<scalar>(s), static_cast<scalar>(s), static_cast<scalar>(s), static_cast<scalar>(s)) {}

        constexpr vector(scalar _x, scalar _y, scalar _z, scalar _w) noexcept : xyzw(_x, _y, _z, _w) {}

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

        L_ALWAYS_INLINE scalar length() const noexcept { return ::legion::core::math::length(*this); }
        constexpr scalar length2() const noexcept { return ::legion::core::math::length2(*this); }
    };

    template<>
    struct alignas(sizeof(bool) * 4) vector<bool, 4>
    {
        using scalar = bool;
        static constexpr size_type size = 4;
        using type = vector<bool, 4>;

        union
        {
            scalar data[4];

            _MATH_SWIZZLE_4_1_(scalar);
            _MATH_SWIZZLE_4_2_(scalar);
            _MATH_SWIZZLE_4_3_(scalar);
            _MATH_SWIZZLE_4_4_(scalar);
        };

        constexpr vector() noexcept
            : xyzw(static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0), static_cast<scalar>(0)) {}

        constexpr vector(const vector&) noexcept = default;

        explicit constexpr vector(scalar s) noexcept
            : xyzw(static_cast<scalar>(s), static_cast<scalar>(s), static_cast<scalar>(s), static_cast<scalar>(s)) {}

        constexpr vector(scalar _x, scalar _y, scalar _z, scalar _w) noexcept : xyzw(_x, _y, _z, _w) {}

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
        static const vector positiveW;
        static const vector negativeW;
        static const vector one;
        static const vector zero;

        constexpr void set_mask(bitfield8 mask) noexcept { x = mask & 1; y = mask & 2; z = mask & 4; w = mask & 8; }
        constexpr bitfield8 mask() const noexcept { return static_cast<bitfield8>(x | y << 1u | z << 2u | w << 3u); }

        constexpr operator bool() const noexcept { return x && y && z && w; }

        constexpr vector& operator=(const vector&) noexcept = default;

        constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
        constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }

        constexpr scalar length() const noexcept { return (x && y && z && w) ? 2 : (x || y || z || w); };
        constexpr scalar length2() const noexcept { return this->length() * this->length(); }
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
    using vec4 = vector<float, 4>;
    using double4 = vector<double, 4>;
    using dvec4 = vector<double, 4>;
    using int4 = vector<int, 4>;
    using ivec4 = vector<int, 4>;
    using bool4 = vector<bool, 4>;
    using bvec4 = vector<bool, 4>;
}
