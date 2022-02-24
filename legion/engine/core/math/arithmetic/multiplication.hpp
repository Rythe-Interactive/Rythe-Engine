#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>
#include <core/math/matrix/matrix.hpp>
#include <core/math/meta.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename T>
        struct compute_multiplication;

        template<typename Scalar, size_type Size>
        struct compute_multiplication<vector<Scalar, Size>>
        {
            using value_type = vector<Scalar, Size>;

            L_NODISCARD constexpr static auto compute(const value_type& a, const value_type& b) noexcept;

            L_NODISCARD constexpr static auto compute(const value_type& a, Scalar b) noexcept;
        };

        template<typename Scalar>
        struct compute_multiplication<vector<Scalar, 1u>>
        {
            using value_type = vector<Scalar, 1u>;

            L_NODISCARD constexpr static Scalar compute(const value_type& a, Scalar b) noexcept;
        };

        template<typename Scalar, size_type SizeH, size_type SizeV>
        struct compute_multiplication<matrix<Scalar, SizeH, SizeV>>
        {
            using value_type = matrix<Scalar, SizeH, SizeV>;

            L_NODISCARD constexpr static auto compute(const value_type& a, const value_type& b) noexcept;

            L_NODISCARD constexpr static auto compute(const value_type& a, typename value_type::row_type b) noexcept;
        };

    }

    // Vector
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

    // Matrix
    template<typename mat_type0, typename mat_type1, std::enable_if_t<is_matrix_v<mat_type0>&& is_matrix_v<mat_type1>, bool> = true>
    L_NODISCARD constexpr auto mul(const mat_type0& a, const mat_type1& b) noexcept;

    template<typename mat_type0, typename mat_type1, std::enable_if_t<is_matrix_v<mat_type0>&& is_matrix_v<mat_type1>, bool> = true>
    L_NODISCARD constexpr auto operator*(const mat_type0& a, const mat_type1& b);

    template<typename mat_type, typename vec_type, std::enable_if_t<is_matrix_v<mat_type>&& is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto mul(const mat_type& a, const vec_type& b) noexcept;

    template<typename mat_type, typename vec_type, std::enable_if_t<is_matrix_v<mat_type>&& is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr auto operator*(const mat_type& a, const vec_type& b);

    template<typename mat_type0, typename mat_type1, std::enable_if_t<is_matrix_v<mat_type0>&& is_matrix_v<mat_type1>, bool> = true>
    constexpr mat_type0& mul_assign(mat_type0& a, const mat_type1& b) noexcept;

    template<typename mat_type0, typename mat_type1, std::enable_if_t<is_matrix_v<mat_type0>&& is_matrix_v<mat_type1>, bool> = true>
    constexpr mat_type0& operator*=(mat_type0& a, const mat_type1& b);
}

#include <core/math/arithmetic/multiplication.inl>
