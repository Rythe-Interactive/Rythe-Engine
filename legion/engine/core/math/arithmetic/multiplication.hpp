#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_multiplication
        {
            using value_type = vector<Scalar, Size>;

            L_NODISCARD constexpr static auto compute(const value_type& a, const value_type& b) noexcept;

            L_NODISCARD constexpr static auto compute(const value_type& a, Scalar b) noexcept;
        };

        template<typename Scalar>
        struct compute_multiplication<Scalar, 1u>
        {
            using value_type = vector<Scalar, 1u>;

            L_NODISCARD constexpr static Scalar compute(const value_type& a, Scalar b) noexcept;
        };
    }

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0> && is_vector_v<vec_type1>, bool> = true>
    L_NODISCARD constexpr auto mul(const vec_type0& a, const vec_type1& b) noexcept;

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0> && is_vector_v<vec_type1>, bool> = true>
    L_NODISCARD constexpr auto operator*(const vec_type0& a, const vec_type1& b);

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto mul(const vec_type& a, typename vec_type::scalar b) noexcept;

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto operator*(const vec_type& a, typename vec_type::scalar b);

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0> && is_vector_v<vec_type1>, bool> = true>
    constexpr vec_type0& mul_assign(vec_type0& a, const vec_type1& b) noexcept;

    template<typename vec_type0, typename vec_type1, std::enable_if_t<is_vector_v<vec_type0> && is_vector_v<vec_type1>, bool> = true>
    constexpr vec_type0& operator*=(vec_type0& a, const vec_type1& b);

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    constexpr vec_type& mul_assign(vec_type& a, typename vec_type::scalar b) noexcept;

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    constexpr vec_type& operator*=(vec_type& a, typename vec_type::scalar b);
}
