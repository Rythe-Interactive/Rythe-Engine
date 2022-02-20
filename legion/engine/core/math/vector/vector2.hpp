#pragma once
#include <core/math/vector/vector_base.hpp>
#include <core/math/vector/swizzle/swizzle2.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    template<typename Scalar>
    struct alignas(sizeof(Scalar) * 2) vector<Scalar, 2>
    {
        static_assert(std::is_arithmetic<Scalar>::value, "Scalar must be a numeric type.");

        using scalar = Scalar;
        static constexpr size_type size = 2;
        using type = vector<Scalar, 2>;

        union
        {
            scalar data[2];

            _MATH_SWIZZLE_2_1_(scalar);
            _MATH_SWIZZLE_2_2_(scalar);
            _MATH_SWIZZLE_2_3_(scalar);
            _MATH_SWIZZLE_2_4_(scalar);
        };

        constexpr vector() noexcept : xy(static_cast<scalar>(0), static_cast<scalar>(0)) {}

        constexpr vector(const vector&) noexcept = default;

        explicit constexpr vector(scalar s) noexcept : xy(static_cast<scalar>(s), static_cast<scalar>(s)) {}

        constexpr vector(scalar _x, scalar _y) noexcept : xy(_x, _y) {}

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
        L_ALWAYS_INLINE scalar angle() const noexcept { return static_cast<scalar>(::std::atan2(y, x)); }
    };

    template<>
    struct alignas(sizeof(bool) * 2) vector<bool, 2>
    {
        using scalar = bool;
        static constexpr size_type size = 2;
        using type = vector<bool, 2>;

        union
        {
            scalar data[2];

            _MATH_SWIZZLE_2_1_(scalar);
            _MATH_SWIZZLE_2_2_(scalar);
            _MATH_SWIZZLE_2_3_(scalar);
            _MATH_SWIZZLE_2_4_(scalar);
        };

        constexpr vector() noexcept : xy(static_cast<scalar>(0), static_cast<scalar>(0)) {}

        constexpr vector(const vector&) noexcept = default;

        explicit constexpr vector(scalar s) noexcept : xy(static_cast<scalar>(s), static_cast<scalar>(s)) {}

        constexpr vector(scalar _x, scalar _y) noexcept : xy(_x, _y) {}

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
        static const vector one;
        static const vector zero;

        constexpr void set_mask(bitfield8 mask) noexcept { x = mask & 1; y = mask & 2; }
        constexpr bitfield8 mask() const noexcept { return static_cast<bitfield8>(x | y << 1u); }

        constexpr operator bool() const noexcept { return x && y; }

        constexpr vector& operator=(const vector&) noexcept = default;

        constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
        constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }

        constexpr scalar length() const noexcept { return x || y; }
        constexpr scalar length2() const noexcept { return this->length(); }
        constexpr scalar angle() const noexcept { return y; }
    };

    template<typename Scalar>
    const vector<Scalar, 2> vector<Scalar, 2>::up(static_cast<Scalar>(0), static_cast<Scalar>(1));
    template<typename Scalar>
    const vector<Scalar, 2> vector<Scalar, 2>::down(static_cast<Scalar>(0), static_cast<Scalar>(-1));
    template<typename Scalar>
    const vector<Scalar, 2> vector<Scalar, 2>::right(static_cast<Scalar>(1), static_cast<Scalar>(0));
    template<typename Scalar>
    const vector<Scalar, 2> vector<Scalar, 2>::left(static_cast<Scalar>(-1), static_cast<Scalar>(0));
    template<typename Scalar>
    const vector<Scalar, 2> vector<Scalar, 2>::one(static_cast<Scalar>(1));
    template<typename Scalar>
    const vector<Scalar, 2> vector<Scalar, 2>::zero(static_cast<Scalar>(0));

    using float2 = vector<float32, 2>;
    using double2 = vector<float64, 2>;
    using int2 = vector<int, 2>;
    using bool2 = vector<bool, 2>;
}
