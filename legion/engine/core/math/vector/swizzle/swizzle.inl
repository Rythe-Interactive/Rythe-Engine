#include <core/math/vector/swizzle/swizzle_base.hpp>
#pragma once

namespace legion::core::math
{
    template<typename scalar, size_type size, size_type s0>
    constexpr swizzle<scalar, size, s0>::swizzle(scalar x) noexcept
    {
        data[s0] = x;
    }

    template<typename scalar, size_type size, size_type s0>
    constexpr swizzle<scalar, size, s0>::operator scalar() const noexcept
    {
        return data[s0];
    }

    template<typename scalar, size_type size, size_type s0>
    constexpr swizzle<scalar, size, s0>::operator conv_type() const noexcept
    {
        conv_type result;
        result.data[0] = data[s0];
        return result;
    }

    template<typename scalar, size_type size, size_type s0>
    constexpr swizzle<scalar, size, s0>& swizzle<scalar, size, s0>::operator=(const conv_type& other) noexcept
    {
        data[s0] = other[0];
        return *this;
    }

    template<typename scalar, size_type size, size_type s0>
    constexpr swizzle<scalar, size, s0>& swizzle<scalar, size, s0>::operator=(scalar value) noexcept
    {
        data[s0] = value;
        return *this;
    }

    template<size_type size, size_type s0>
    constexpr swizzle<bool, size, s0>::swizzle(bool x) noexcept
    {
        data[s0] = x;
    }

    template<size_type size, size_type s0>
    constexpr void swizzle<bool, size, s0>::set_mask(bitfield8 mask) noexcept
    {
        data[s0] = mask & 1;
    }

    template<size_type size, size_type s0>
    constexpr bitfield8 swizzle<bool, size, s0>::mask() const noexcept
    {
        return static_cast<bitfield8>(data[s0]);
    }

    template<size_type size, size_type s0>
    constexpr swizzle<bool, size, s0>::operator scalar() const noexcept
    {
        return data[s0];
    }

    template<size_type size, size_type s0>
    constexpr swizzle<bool, size, s0>::operator conv_type() const noexcept
    {
        conv_type result;
        result.data[0] = data[s0];
        return result;
    }

    template<size_type size, size_type s0>
    constexpr swizzle<bool, size, s0>& swizzle<bool, size, s0>::operator=(const conv_type& other) noexcept
    {
        data[s0] = other[0];
        return *this;
    }

    template<size_type size, size_type s0>
    constexpr swizzle<bool, size, s0>& swizzle<bool, size, s0>::operator=(scalar value) noexcept
    {
        data[s0] = value;
        return *this;
    }

    template<typename scalar, size_type size, size_type s0, size_type s1>
    constexpr swizzle<scalar, size, s0, s1>::swizzle(scalar x, scalar y) noexcept
    {
        data[s0] = x;
        data[s1] = y;
    }

    template<typename scalar, size_type size, size_type s0, size_type s1>
    constexpr swizzle<scalar, size, s0, s1>::operator conv_type() const noexcept
    {
        conv_type result;
        result.data[0] = data[s0];
        result.data[1] = data[s1];
        return result;
    }

    template<typename scalar, size_type size, size_type s0, size_type s1>
    constexpr swizzle<scalar, size, s0, s1>& swizzle<scalar, size, s0, s1>::operator=(const conv_type& other) noexcept
    {
        data[s0] = other[0];
        data[s1] = other[1];
        return *this;
    }

    template<size_type size, size_type s0, size_type s1>
    constexpr swizzle<bool, size, s0, s1>::swizzle(bool x, bool y) noexcept
    {
        data[s0] = x;
        data[s1] = y;
    }

    template<size_type size, size_type s0, size_type s1>
    constexpr void swizzle<bool, size, s0, s1>::set_mask(bitfield8 mask) noexcept
    {
        data[s0] = mask & 1;
        data[s1] = mask & 2;
    }

    template<size_type size, size_type s0, size_type s1>
    constexpr bitfield8 swizzle<bool, size, s0, s1>::mask() const noexcept
    {
        return static_cast<bitfield8>(data[s0] | data[s0] << 1u);
    }

    template<size_type size, size_type s0, size_type s1>
    constexpr swizzle<bool, size, s0, s1>::operator conv_type() const noexcept
    {
        conv_type result;
        result.data[0] = data[s0];
        result.data[1] = data[s1];
        return result;
    }

    template<size_type size, size_type s0, size_type s1>
    constexpr swizzle<bool, size, s0, s1>& swizzle<bool, size, s0, s1>::operator=(const conv_type& other) noexcept
    {
        data[s0] = other[0];
        data[s1] = other[1];
        return *this;
    }

    template<typename scalar, size_type size, size_type s0, size_type s1, size_type s2>
    constexpr swizzle<scalar, size, s0, s1, s2>::swizzle(scalar x, scalar y, scalar z) noexcept
    {
        data[s0] = x;
        data[s1] = y;
        data[s2] = z;
    }

    template<typename scalar, size_type size, size_type s0, size_type s1, size_type s2>
    constexpr swizzle<scalar, size, s0, s1, s2>::operator conv_type() const noexcept
    {
        conv_type result;
        result.data[0] = data[s0];
        result.data[1] = data[s1];
        result.data[2] = data[s2];
        return result;
    }

    template<typename scalar, size_type size, size_type s0, size_type s1, size_type s2>
    constexpr swizzle<scalar, size, s0, s1, s2>& swizzle<scalar, size, s0, s1, s2>::operator=(const conv_type& other) noexcept
    {
        data[s0] = other[0];
        data[s1] = other[1];
        data[s2] = other[2];
        return *this;
    }

    template<size_type size, size_type s0, size_type s1, size_type s2>
    constexpr swizzle<bool, size, s0, s1, s2>::swizzle(bool x, bool y, bool z) noexcept
    {
        data[s0] = x;
        data[s1] = y;
        data[s2] = z;
    }

    template<size_type size, size_type s0, size_type s1, size_type s2>
    constexpr void swizzle<bool, size, s0, s1, s2>::set_mask(bitfield8 mask) noexcept
    {
        data[s0] = mask & 1;
        data[s1] = mask & 2;
        data[s2] = mask & 4;
    }

    template<size_type size, size_type s0, size_type s1, size_type s2>
    constexpr bitfield8 swizzle<bool, size, s0, s1, s2>::mask() const noexcept
    {
        return static_cast<bitfield8>(data[s0] | data[s1] << 1u | data[s2] << 2u);
    }

    template<size_type size, size_type s0, size_type s1, size_type s2>
    constexpr swizzle<bool, size, s0, s1, s2>::operator conv_type() const noexcept
    {
        conv_type result;
        result.data[0] = data[s0];
        result.data[1] = data[s1];
        result.data[2] = data[s2];
        return result;
    }

    template<size_type size, size_type s0, size_type s1, size_type s2>
    constexpr swizzle<bool, size, s0, s1, s2>& swizzle<bool, size, s0, s1, s2>::operator=(const conv_type& other) noexcept
    {
        data[s0] = other[0];
        data[s1] = other[1];
        data[s2] = other[2];
        return *this;
    }

    template<typename scalar, size_type size, size_type s0, size_type s1, size_type s2, size_type s3>
    constexpr swizzle<scalar, size, s0, s1, s2, s3>::swizzle(scalar x, scalar y, scalar z, scalar w) noexcept
    {
        data[s0] = x;
        data[s1] = y;
        data[s2] = z;
        data[s3] = w;
    }

    template<typename scalar, size_type size, size_type s0, size_type s1, size_type s2, size_type s3>
    constexpr swizzle<scalar, size, s0, s1, s2, s3>::operator conv_type() const noexcept
    {
        conv_type result;
        result.data[0] = data[s0];
        result.data[1] = data[s1];
        result.data[2] = data[s2];
        result.data[3] = data[s3];
        return result;
    }

    template<typename scalar, size_type size, size_type s0, size_type s1, size_type s2, size_type s3>
    constexpr swizzle<scalar, size, s0, s1, s2, s3>& swizzle<scalar, size, s0, s1, s2, s3>::operator=(const conv_type& other) noexcept
    {
        data[s0] = other[0];
        data[s1] = other[1];
        data[s2] = other[2];
        data[s3] = other[3];
        return *this;
    }

    template<size_type size, size_type s0, size_type s1, size_type s2, size_type s3>
    constexpr swizzle<bool, size, s0, s1, s2, s3>::swizzle(bool x, bool y, bool z, bool w) noexcept
    {
        data[s0] = x;
        data[s1] = y;
        data[s2] = z;
        data[s3] = w;
    }

    template<size_type size, size_type s0, size_type s1, size_type s2, size_type s3>
    constexpr void swizzle<bool, size, s0, s1, s2, s3>::set_mask(bitfield8 mask) noexcept
    {
        data[s0] = mask & 1;
        data[s1] = mask & 2;
        data[s2] = mask & 4;
        data[s3] = mask & 8;
    }

    template<size_type size, size_type s0, size_type s1, size_type s2, size_type s3>
    constexpr bitfield8 swizzle<bool, size, s0, s1, s2, s3>::mask() const noexcept
    {
        return static_cast<bitfield8>(data[s0] | data[s1] << 1u | data[s2] << 2u | data[s3] << 3u);
    }

    template<size_type size, size_type s0, size_type s1, size_type s2, size_type s3>
    constexpr swizzle<bool, size, s0, s1, s2, s3>::operator conv_type() const noexcept
    {
        conv_type result;
        result.data[0] = data[s0];
        result.data[1] = data[s1];
        result.data[2] = data[s2];
        result.data[3] = data[s3];
        return result;
    }

    template<size_type size, size_type s0, size_type s1, size_type s2, size_type s3>
    constexpr swizzle<bool, size, s0, s1, s2, s3>& swizzle<bool, size, s0, s1, s2, s3>::operator=(const conv_type& other) noexcept
    {
        data[s0] = other[0];
        data[s1] = other[1];
        data[s2] = other[2];
        data[s3] = other[3];
        return *this;
    }
}
