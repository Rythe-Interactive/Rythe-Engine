#pragma once
#include <cmath>
#include <limits>

#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Target, typename T>
        L_NODISCARD constexpr static Target _round_impl_(T val)
        {
            using value_type = remove_cvr_t<T>;
            if (val < static_cast<value_type>(0))
            {
                return static_cast<Target>(static_cast<int_max>(val - static_cast<value_type>(0.5)));
            }
            else
            {
                return static_cast<Target>(static_cast<int_max>(val + static_cast<value_type>(0.5)));
            }
        }

        template<typename Scalar, size_type Size>
        struct compute_round
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            L_NODISCARD constexpr static value_type compute(const value_type& val) noexcept
            {
                value_type result;
                for (size_type i = 0; i < size; i++)
                    result[i] = detail::_round_impl_<Scalar>(val[i]);
                return result;
            }

            template<typename Integer>
            L_NODISCARD constexpr static vector<Integer, size> icompute(const value_type& val) noexcept
            {
                vector<Integer, size> result;
                for (size_type i = 0; i < size; i++)
                    result[i] = detail::_round_impl_<Integer>(val[i]);
                return result;
            }
        };

        template<typename Scalar>
        struct compute_round<Scalar, 1u>
        {
            static constexpr size_type size = 1u;
            using value_type = vector<Scalar, size>;

            L_NODISCARD constexpr static Scalar compute(const value_type& val) noexcept
            {
                return detail::_round_impl_<Scalar>(val[0]);
            }

            template<typename Integer>
            L_NODISCARD constexpr static Integer icompute(const value_type& val) noexcept
            {
                return detail::_round_impl_<Integer>(val[0]);
            }
        };
    }

    template<typename T>
    L_NODISCARD constexpr static auto round(T val)
    {
        using value_type = remove_cvr_t<T>;
        if constexpr (is_vector_v<value_type>)
        {
            return detail::compute_round<typename value_type::scalar, value_type::size>::compute(val);
        }
        else
        {
            return detail::compute_round<value_type, 1>::compute(val);
        }
    }

    template<typename Integer = int, typename T = float>
    L_NODISCARD constexpr static auto iround(T val)
    {
        using value_type = remove_cvr_t<T>;
        if constexpr (is_vector_v<value_type>)
        {
            return detail::compute_round<typename value_type::scalar, value_type::size>::template icompute<Integer>(val);
        }
        else
        {
            return detail::compute_round<value_type, 1u>::template icompute<Integer>(val);
        }
    }
}
