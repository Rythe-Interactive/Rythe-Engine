#pragma once
/*********************************************************************
 * @file   core/math/util/hash.hpp
 * @date   05-12-2021
 * @brief  Hashing functions and utilities for math structures.
 * 
 * Copyright (c) 2021 Rythe-Interactive All rights reserved
 *********************************************************************/

#include <functional>
#include <core/types/primitives.hpp>
#include <core/types/hash.hpp>
#include <core/math/vector/vector.hpp>

template<typename _Scalar, ::std::size_t _Size, ::std::size_t... args>
struct ::std::hash<::legion::core::math::swizzle<_Scalar, _Size, args...>>
{
    constexpr static ::std::hash<typename ::legion::core::math::swizzle<_Scalar, _Size, args...>::conv_type> hasher;
    inline L_ALWAYS_INLINE ::std::size_t operator()(const ::legion::core::math::swizzle<_Scalar, _Size, args...>& vec) const noexcept
    {
        using namespace ::legion::core;
        return hasher(vec);
    }
};

template<typename _Scalar, ::std::size_t _Size>
struct ::std::hash<::legion::core::math::vector<_Scalar, _Size>>
{
    constexpr static ::std::hash<_Scalar> hasher;
    inline L_ALWAYS_INLINE ::std::size_t operator()(const ::legion::core::math::vector<_Scalar, _Size>& vec) const noexcept
    {
        using namespace ::legion::core;
        size_type seed = hasher(vec[0]);
        for (size_type i = 1; i < _Size; i++)
            seed = hash_combine(seed, hasher(vec[i]));
        return seed;
    }
};

template<typename _Scalar>
struct ::std::hash<::legion::core::math::vector<_Scalar, 1>>
{
    constexpr static ::std::hash<_Scalar> hasher;
    inline L_ALWAYS_INLINE ::std::size_t operator()(const ::legion::core::math::vector<_Scalar, 1>& vec) const noexcept
    {
        return hasher(vec.x);
    }
};

template<typename _Scalar>
struct ::std::hash<::legion::core::math::vector<_Scalar, 2>>
{
    constexpr static ::std::hash<_Scalar> hasher;
    inline L_ALWAYS_INLINE ::std::size_t operator()(const ::legion::core::math::vector<_Scalar, 2>& vec) const noexcept
    {
        return ::legion::core::hash_combine(hasher(vec.x), hasher(vec.y));
    }
};

template<typename _Scalar>
struct ::std::hash<::legion::core::math::vector<_Scalar, 3>>
{
    constexpr static ::std::hash<_Scalar> hasher;
    inline L_ALWAYS_INLINE ::std::size_t operator()(const ::legion::core::math::vector<_Scalar, 3>& vec) const noexcept
    {
        return ::legion::core::hash_combine_multiple(hasher(vec.x), hasher(vec.y), hasher(vec.z));
    }
};

template<typename _Scalar>
struct ::std::hash<::legion::core::math::vector<_Scalar, 4>>
{
    constexpr static ::std::hash<_Scalar> hasher;
    inline L_ALWAYS_INLINE ::std::size_t operator()(const ::legion::core::math::vector<_Scalar, 4>& vec) const noexcept
    {
        return ::legion::core::hash_combine_multiple(hasher(vec.x), hasher(vec.y), hasher(vec.z), hasher(vec.w));
    }
};

