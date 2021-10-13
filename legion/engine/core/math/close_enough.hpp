/**
 * Copyright 2020 Raphael Baier
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#pragma once
#include <cstdlib>
#include <numeric>
#include <limits>
#include <cmath>
#include <type_traits>

#include <core/math/glm/glm_include.hpp>

namespace legion::core::math {

    //get the highest epsilon for N amount of floats
    template <class... FLTS>
    constexpr auto get_highest_epsilon(FLTS... flts)
    {
        static_assert((std::is_floating_point<FLTS>::value && ...) == true,
                      "get highest epsilon is only useful if all numbers are floating point");

        if constexpr ((std::is_same<typename std::remove_cv<FLTS>::type,float>::value || ...) == true)
            return std::numeric_limits<float>::epsilon();
        else if constexpr ((std::is_same<typename std::remove_cv<FLTS>::type,double>::value || ...) == true)
            return std::numeric_limits<double>::epsilon();
        else
            return std::numeric_limits<long double>::epsilon();
    }

    //get the least precise type from a bunch of floating points
    template <class... FLTS>
    struct lowest_precision
    {
        using type = decltype(get_highest_epsilon(std::declval<FLTS>()...));
    };

    //check if two types are nearly equal
    template <class NUM_TYPE1,class NUM_TYPE2>
    constexpr bool close_enough(NUM_TYPE1 lhs, NUM_TYPE2 rhs)
    {
        if constexpr(std::numeric_limits<NUM_TYPE1>::is_integer &&
                     std::numeric_limits<NUM_TYPE2>::is_integer)
            return lhs == rhs;

        else if constexpr(std::numeric_limits<NUM_TYPE1>::is_integer)
            return abs(NUM_TYPE2(lhs) - rhs) <= std::numeric_limits<NUM_TYPE2>::epsilon();

        else if constexpr(std::numeric_limits<NUM_TYPE2>::is_integer)
            return abs(lhs - NUM_TYPE1(rhs)) <= std::numeric_limits<NUM_TYPE1>::epsilon();

        else {
            using lowest_t = typename lowest_precision<NUM_TYPE1,NUM_TYPE2>::type;
            return abs(lowest_t(lhs) - lowest_t(rhs)) <= get_highest_epsilon(lhs,rhs);
        }
    }

    namespace detail
    {
        template <>
        struct compute_equal<float, true>
        {
            GLM_FUNC_QUALIFIER GLM_CONSTEXPR static bool call(float a, float b)
            {
                return close_enough(a, b);
            }
        };

        template <typename T>
        struct compute_equal<T, true>
        {
            GLM_FUNC_QUALIFIER GLM_CONSTEXPR static bool call(T a, T b)
            {
                return close_enough(a, b);
            }
        };
    }
}
