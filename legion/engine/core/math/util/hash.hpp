#pragma once
#include <functional>
#include <core/types/primitives.hpp>
#include <core/math/vector/vector.hpp>

namespace legion::core::math
{
    constexpr static void hash_combine(size_type& seed, size_type hash)
    {
        hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hash;
    }   
}

template<typename _Scalar, ::std::size_t _Size>
struct ::std::hash<::legion::core::math::vector<_Scalar, _Size>>
{
    constexpr ::std::size_t operator()(const ::legion::core::math::vector<_Scalar, _Size>& vec)
    {
        using namespace ::legion::core;
        size_type seed = 0;
        ::std::hash<_Scalar> hasher;
        for (size_type i = 0; i < _Size; i++)
            hash_combine(seed, hasher(vec[i]));
        return seed;
    }
};
