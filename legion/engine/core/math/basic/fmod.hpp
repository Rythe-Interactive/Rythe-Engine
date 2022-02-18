#pragma once
#include <cmath>
#include <limits>
#include <core/types/primitives.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_mod
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            inline L_ALWAYS_INLINE static value_type computef(const value_type& val, Scalar m) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = ::std::fmod(val[i], m);
                return result;
            }

            inline L_ALWAYS_INLINE static value_type computef(const value_type& val, const value_type& m) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = ::std::fmod(val[i], m[i]);
                return result;
            }

            constexpr static value_type compute(const value_type& val, Scalar m) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = val[i] % m;
                return result;
            }

            constexpr static value_type compute(const value_type& val, const value_type& m) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = val[i] % m[i];
                return result;
            }
        };
    }

    template<typename T>
    inline L_ALWAYS_INLINE static T fmod(const T& val, const T& m)
    {
        if constexpr (is_vector_v<T>)
        {
            static_assert(::std::is_floating_point_v<typename T::scalar>, "Value must be floating point in order to use fmod, use mod instead.");
            return detail::compute_mod<typename T::scalar, T::size>::computef(val, m);
        }
        else
        {
            static_assert(::std::is_floating_point_v<T>, "Value must be floating point in order to use fmod, use mod instead.");
            return ::std::fmod(val, m);
        }
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    inline L_ALWAYS_INLINE static auto fmod(const vec_type& val, typename vec_type::scalar m)
    {
        static_assert(::std::is_floating_point_v<typename vec_type::scalar>, "Value must be floating point in order to use fmod, use mod instead.");
        return detail::compute_mod<typename vec_type::scalar, vec_type::size>::computef(val, m);
    }

    template<typename T>
    inline L_ALWAYS_INLINE static T mod(const T& val, const T& m)
    {
        if constexpr (is_vector_v<T>)
        {
            if constexpr (::std::is_floating_point_v<typename T::scalar>)
                return detail::compute_mod<typename T::scalar, T::size>::computef(val, m);
            else
                return detail::compute_mod<typename T::scalar, T::size>::compute(val, m);
        }
        else if constexpr (::std::is_floating_point_v<T>)
            return ::std::fmod(val, m);
        else
            return val % m;
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    inline L_ALWAYS_INLINE static auto mod(const vec_type& val, typename vec_type::scalar m)
    {
        if constexpr (::std::is_floating_point_v<typename vec_type::scalar>)
            return detail::compute_mod<typename vec_type::scalar, vec_type::size>::computef(val, m);
        else
            return detail::compute_mod<typename vec_type::scalar, vec_type::size>::compute(val, m);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    inline L_ALWAYS_INLINE auto operator%(const vec_type& val, typename vec_type::scalar m)
    {
        return mod(val, m);
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    inline L_ALWAYS_INLINE auto operator%(const vec_type& val, const vec_type& m)
    {
        return mod(val, m);
    }
}
