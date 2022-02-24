#pragma once
#include <core/types/primitives.hpp>
#include <core/math/vector/vector_base.hpp>

namespace legion::core::math
{
    template<typename Scalar, size_type SizeH, size_type SizeV>
    struct matrix
    {
        static_assert(std::is_arithmetic_v<Scalar>, "Scalar must be a numeric type.");

        using scalar = Scalar;
        static constexpr size_type size = SizeH * SizeV;
        static constexpr size_type size_h = SizeH;
        static constexpr size_type size_v = SizeV;
        using type = matrix<Scalar, SizeH, SizeV>;

        using row_type = vector<scalar, size_h>;

        union
        {
            row_type rows[size_v];
            scalar data[size_h * size_v];
        };

        static const matrix identity;

        constexpr matrix() noexcept;

        constexpr matrix(const matrix&) noexcept = default;
        explicit constexpr matrix(scalar s) noexcept;

        template<typename Scal, ::std::enable_if_t<!::std::is_same_v<scalar, Scal>, bool> = true>
        constexpr explicit matrix(const matrix<Scal, size_h, size_v>&other) noexcept;

        template<typename mat_type, ::std::enable_if_t<SizeH != mat_type::size_h || SizeV != mat_type::size_v, bool> = true>
        constexpr matrix(const mat_type& other) noexcept;

        constexpr matrix& operator=(const matrix&) noexcept = default;

        L_NODISCARD constexpr row_type& operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL);
        L_NODISCARD constexpr const row_type& operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL);
    };
}
