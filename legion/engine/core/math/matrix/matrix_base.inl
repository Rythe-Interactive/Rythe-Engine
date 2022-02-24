#include <core/math/matrix/matrix_base.hpp>
#pragma once

namespace legion::core::math
{
    template<typename Scalar, size_type SizeH, size_type SizeV>
    const matrix<Scalar, SizeH, SizeV> matrix<Scalar, SizeH, SizeV>::identity = matrix<Scalar, SizeH, SizeV>(static_cast<Scalar>(1));

    template<typename Scalar, size_type SizeH, size_type SizeV>
    constexpr matrix<Scalar, SizeH, SizeV>::matrix() noexcept
    {
        for (size_type i = 0; i < size_v; i++)
            for (size_type j = 0; j < size_h; j++)
                rows[i][j] = i == j ? static_cast<scalar>(1) : static_cast<scalar>(0);
    }

    template<typename Scalar, size_type SizeH, size_type SizeV>
    constexpr matrix<Scalar, SizeH, SizeV>::matrix(scalar s) noexcept
    {
        for (size_type i = 0; i < size_v; i++)
            for (size_type j = 0; j < size_h; j++)
                rows[i][j] = i == j ? s : static_cast<scalar>(0);
    }

    template<typename Scalar, size_type SizeH, size_type SizeV>
    template<typename Scal, ::std::enable_if_t<!::std::is_same_v<Scalar, Scal>, bool>>
    constexpr matrix<Scalar, SizeH, SizeV>::matrix(const matrix<Scal, size_h, size_v>& other) noexcept
    {
        for (size_type i = 0; i < size; i++)
            data[i] = static_cast<scalar>(other.data[i]);
    }

    template<typename Scalar, size_type SizeH, size_type SizeV>
    template<typename mat_type, ::std::enable_if_t<SizeH != mat_type::size_h || SizeV != mat_type::size_v, bool>>
    constexpr matrix<Scalar, SizeH, SizeV>::matrix(const mat_type& other) noexcept
    {
        constexpr size_type min_h = size_h < mat_type::size_h ? size_h : mat_type::size_h;
        constexpr size_type min_v = size_v < mat_type::size_v ? size_v : mat_type::size_v;

        for (size_type i = 0; i < min_v; i++)
            for (size_type j = 0; j < min_h; j++)
                rows[i][j] = static_cast<scalar>(other.mx[i][j]);
    }

    template<typename Scalar, size_type SizeH, size_type SizeV>
    L_NODISCARD constexpr typename matrix<Scalar, SizeH, SizeV>::row_type& matrix<Scalar, SizeH, SizeV>::operator[](size_type i) noexcept(!LEGION_VALIDATION_LEVEL)
    {
        assert_msg("matrix subscript out of range", (i >= 0) && (i < size_v)); return rows[i];
    }

    template<typename Scalar, size_type SizeH, size_type SizeV>
    L_NODISCARD constexpr const typename matrix<Scalar, SizeH, SizeV>::row_type& matrix<Scalar, SizeH, SizeV>::operator[](size_type i) const noexcept(!LEGION_VALIDATION_LEVEL)
    {
        assert_msg("matrix subscript out of range", (i >= 0) && (i < size_v)); return rows[i];
    }
}
