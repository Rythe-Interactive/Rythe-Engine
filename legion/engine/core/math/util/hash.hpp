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

template<typename Scalar, ::std::size_t Size>
struct ::std::hash<::legion::core::math::vector<Scalar, Size>>
{
    constexpr static ::std::hash<Scalar> hasher;
    inline L_ALWAYS_INLINE::std::size_t operator()(const ::legion::core::math::vector<Scalar, Size>& vec) const noexcept
    {
        using namespace ::legion::core;
        size_type seed = hasher(vec[0]);
        for (size_type i = 1; i < Size; i++)
            seed = ::legion::core::hash_combine(seed, hasher(vec[i]));
        return seed;
    }
};

template<typename Scalar>
struct ::std::hash<::legion::core::math::vector<Scalar, 1>>
{
    constexpr static ::std::hash<Scalar> hasher;
    inline L_ALWAYS_INLINE::std::size_t operator()(const ::legion::core::math::vector<Scalar, 1>& vec) const noexcept
    {
        return hasher(vec.x);
    }
};

template<typename Scalar>
struct ::std::hash<::legion::core::math::vector<Scalar, 2>>
{
    constexpr static ::std::hash<Scalar> hasher;
    inline L_ALWAYS_INLINE::std::size_t operator()(const ::legion::core::math::vector<Scalar, 2>& vec) const noexcept
    {
        return ::legion::core::hash_combine(hasher(vec.x), hasher(vec.y));
    }
};

template<typename Scalar>
struct ::std::hash<::legion::core::math::vector<Scalar, 3>>
{
    constexpr static ::std::hash<Scalar> hasher;
    inline L_ALWAYS_INLINE::std::size_t operator()(const ::legion::core::math::vector<Scalar, 3>& vec) const noexcept
    {
        return ::legion::core::hash_combine_multiple(hasher(vec.x), hasher(vec.y), hasher(vec.z));
    }
};

template<typename Scalar>
struct ::std::hash<::legion::core::math::vector<Scalar, 4>>
{
    constexpr static ::std::hash<Scalar> hasher;
    inline L_ALWAYS_INLINE::std::size_t operator()(const ::legion::core::math::vector<Scalar, 4>& vec) const noexcept
    {
        return ::legion::core::hash_combine_multiple(hasher(vec.x), hasher(vec.y), hasher(vec.z), hasher(vec.w));
    }
};

template<typename Scalar, ::std::size_t Size, ::std::size_t... args>
struct ::std::hash<::legion::core::math::swizzle<Scalar, Size, args...>>
{
    constexpr static ::std::hash<typename ::legion::core::math::swizzle<Scalar, Size, args...>::conv_type> hasher;
    inline L_ALWAYS_INLINE::std::size_t operator()(const ::legion::core::math::swizzle<Scalar, Size, args...>& vec) const noexcept
    {
        using namespace ::legion::core;
        return hasher(vec);
    }
};
