#pragma once
/*********************************************************************
 * @file   hash.hpp
 * @date   04-12-2021
 * @brief  
 * 
 * Copyright (c) 2021 Rythe-Interactive All rights reserved
 *********************************************************************/

#include <functional>
#include <core/types/primitives.hpp>

/**
 * @brief .
 */
namespace legion::core
{
    constexpr static void hash_combine(size_type& seed, size_type hash)
    {
        hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hash;
    }

    /**
     * @brief .
     * 
     * @param val
     * @return 
     */
    template<typename T>
    constexpr static size_type hash(const T& val)
    {
        constexpr ::std::hash<T> hasher{};
        return hasher(val);
    }
}
