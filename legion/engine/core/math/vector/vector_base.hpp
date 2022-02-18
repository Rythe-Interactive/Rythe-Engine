#pragma once
#include <tuple>
#include <core/common/assert.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>

namespace legion::core::math
{
    template<typename Scalar, size_type Size>
    struct vector;

    template<typename vec_type>
    typename vec_type::scalar length(const vec_type& v) noexcept;

    template<typename vec_type>
    constexpr typename vec_type::scalar length2(const vec_type& v) noexcept;

    template<typename Scalar, size_type Size>
    struct vector
    {
        static_assert(std::is_arithmetic<Scalar>::value, "Scalar must be a numeric type.");

        using scalar = Scalar;
        static constexpr size_type size = Size;
        using type = vector<Scalar, Size>;

        scalar data[size];

        constexpr vector() noexcept { for (size_type i = 0; i < size; i++) data[i] = static_cast<scalar>(0); }
        constexpr vector(const vector&) noexcept = default;
        explicit constexpr vector(scalar s) noexcept { for (size_type i = 0; i < size; i++) data[i] = s; }

        template<typename _Scal, ::std::enable_if_t<!::std::is_same_v<scalar, _Scal>, bool> = true>
        constexpr explicit vector(const vector<_Scal, size>& other) noexcept
        {
            for (size_type i = 0; i < size; i++) data[i] = static_cast<scalar>(other.data[i]);
        }

        template<typename vec_type, ::std::enable_if_t<size != vec_type::size, bool> = true>
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

        static const vector one;
        static const vector zero;

        constexpr vector& operator=(const vector&) noexcept = default;

        L_NODISCARD constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
        L_NODISCARD constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }

        L_NODISCARD L_ALWAYS_INLINE scalar length() const noexcept { return ::legion::core::math::length(*this); }
        L_NODISCARD constexpr scalar length2() const noexcept { return ::legion::core::math::length2(*this); }
    };

    template<typename Scalar, size_type Size>
    const vector<Scalar, Size> vector<Scalar, Size>::one(static_cast<Scalar>(1));

    template<typename Scalar, size_type Size>
    const vector<Scalar, Size> vector<Scalar, Size>::zero(static_cast<Scalar>(0));

    template<size_type I, typename Scalar, size_type Size>
    inline L_ALWAYS_INLINE Scalar& get(vector<Scalar, Size>& val) { return val[I]; }
}

template<::std::size_t I, typename Scalar, ::std::size_t Size>
struct ::std::tuple_element<I, ::legion::core::math::vector<Scalar, Size>> { using type = Scalar; };

namespace std
{
    template<::std::size_t I, typename Scalar, ::std::size_t Size>
    inline L_ALWAYS_INLINE Scalar& get(::legion::core::math::vector<Scalar, Size>& val) { return ::legion::core::math::get<I>(val); }
}

template<typename Scalar, ::std::size_t Size>
struct ::std::tuple_size<::legion::core::math::vector<Scalar, Size>>
    : public std::integral_constant<::std::size_t, Size> {};

template<typename Scalar, ::std::size_t Size>
struct ::std::tuple_size<const ::legion::core::math::vector<Scalar, Size>>
    : public std::integral_constant<::std::size_t, Size> {};

template<typename Scalar, ::std::size_t Size>
struct ::std::tuple_size<volatile ::legion::core::math::vector<Scalar, Size>>
    : public std::integral_constant<::std::size_t, Size> {};

template<typename Scalar, ::std::size_t Size>
struct ::std::tuple_size<const volatile ::legion::core::math::vector<Scalar, Size>>
    : public std::integral_constant<::std::size_t, Size> {};
