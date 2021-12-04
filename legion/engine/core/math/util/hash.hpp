#pragma once
#include <functional>
#include <core/types/primitives.hpp>
#include <core/types/hash.hpp>
#include <core/math/vector/vector.hpp>

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
