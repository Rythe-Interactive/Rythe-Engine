#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    namespace detail
    {
        template<typename Scalar, size_type Size>
        struct compute_all
        {
            static constexpr size_type size = Size;
            using value_type = vector<Scalar, size>;

            L_NODISCARD constexpr static bool compute(const value_type& value) noexcept
            {
                for (size_type i = 0; i < size; i++)
                    if (!value[i])
                        return false;
                return true;
            }
        };
    }

    template<typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
    L_NODISCARD constexpr bool all(const vec_type& value) noexcept
    {
        return detail::compute_all<typename vec_type::scalar, vec_type::size>::compute(value);
    }
}
