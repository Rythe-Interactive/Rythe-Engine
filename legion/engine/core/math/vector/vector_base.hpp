#pragma once
#include <tuple>
#include <core/common/assert.hpp>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>

namespace legion::core::math
{
    template<typename _Scalar, size_type _Size>
    struct vector;

    template<typename vec_type>
    typename vec_type::scalar length(const vec_type& v) noexcept;

    template<typename vec_type>
    constexpr typename vec_type::scalar length2(const vec_type& v) noexcept;

    namespace detail
    {
        template<size_type _Counter, typename _Scalar, size_type _Size, typename... _Scalars>
        constexpr make_sequence_t<::std::tuple, _Scalar, _Size> _construct_tuple_for_array_impl_(const _Scalar(&arr)[_Size], _Scalars... items) noexcept
        {
            if constexpr (_Counter < _Size)
            {
                return _construct_tuple_for_array_impl_<_Counter + 1, _Scalar, _Size>(arr, items..., arr[_Counter]);
            }
            else
            {
                return ::std::make_tuple(items...);
            }
        }


        template<typename _Scalar, size_type _Size>
        constexpr make_sequence_t<::std::tuple, _Scalar, _Size> _make_tuple_from_array_(const _Scalar(&arr)[_Size]) noexcept
        {
            return _construct_tuple_for_array_impl_<0, _Scalar, _Size>(arr);
        }
    }

    template<typename _Scalar, size_type _Size>
    struct vector
    {
        static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

        using scalar = _Scalar;
        static constexpr size_type size = _Size;
        using type = vector<_Scalar, _Size>;

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

        L_NODISCARD constexpr operator typename make_sequence<::std::tuple, _Scalar, _Size>::type() const noexcept
        {
            return detail::_make_tuple_from_array_<scalar, size>(data);
        }

        template<size_type I>
        L_NODISCARD constexpr scalar& get() noexcept { return data[I]; }
        template<size_type I>
        L_NODISCARD constexpr const scalar& get() const noexcept { return data[I]; }

        L_NODISCARD constexpr scalar& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }
        L_NODISCARD constexpr const scalar& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
        {
            assert_msg("vector subscript out of range", (i >= 0) && (i < size)); return data[i];
        }

        L_ALWAYS_INLINE L_NODISCARD scalar length() const noexcept { return ::legion::core::math::length(*this); }
        L_NODISCARD constexpr scalar length2() const noexcept { return ::legion::core::math::length2(*this); }
    };

    template<typename _Scalar, size_type _Size>
    const vector<_Scalar, _Size> vector<_Scalar, _Size>::one(static_cast<_Scalar>(1));

    template<typename _Scalar, size_type _Size>
    const vector<_Scalar, _Size> vector<_Scalar, _Size>::zero(static_cast<_Scalar>(0));
}

template<::std::size_t I, typename _Scalar, ::std::size_t _Size>
struct ::std::tuple_element<I, ::legion::core::math::vector<_Scalar, _Size>> { using type = _Scalar; };

namespace std
{
    template<::std::size_t I, typename _Scalar, ::std::size_t _Size>
    _Scalar& get(::legion::core::math::vector<_Scalar, _Size>& val) { return val[I]; }
}

template<typename _Scalar, ::std::size_t _Size>
struct ::std::tuple_size<::legion::core::math::vector<_Scalar, _Size>>
    : public std::integral_constant<::std::size_t, _Size> {};

template<typename _Scalar, ::std::size_t _Size>
struct ::std::tuple_size<const ::legion::core::math::vector<_Scalar, _Size>>
    : public std::integral_constant<::std::size_t, _Size> {};

template<typename _Scalar, ::std::size_t _Size>
struct ::std::tuple_size<volatile ::legion::core::math::vector<_Scalar, _Size>>
    : public std::integral_constant<::std::size_t, _Size> {};

template<typename _Scalar, ::std::size_t _Size>
struct ::std::tuple_size<const volatile ::legion::core::math::vector<_Scalar, _Size>>
    : public std::integral_constant<::std::size_t, _Size> {};
