#pragma once

#include <core/math/vector/swizzle/swizzle_base.hpp>

#define _MATH_SWIZZLE_2_2_(type)\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 0> xx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 1> yy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 1> xy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 0> yx;};\

#define _MATH_SWIZZLE_2_3_(type)\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 0, 0> xxx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 1, 1> yyy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 0, 1> xxy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 1, 0> xyx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 1, 1> xyy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 0, 0> yxx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 0, 1> yxy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 1, 0> yyx;};\

#define _MATH_SWIZZLE_2_4_(type)\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 0, 0, 0> xxxx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 1, 1, 1> yyyy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 0, 0, 1> xxxy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 0, 1, 0> xxyx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 0, 1, 1> xxyy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 1, 0, 0> xyxx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 1, 0, 1> xyxy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 1, 1, 0> xyyx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 0, 1, 1, 1> xyyy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 0, 0, 0> yxxx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 0, 0, 1> yxxy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 0, 1, 0> yxyx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 0, 1, 1> yxyy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 1, 0, 0> yyxx;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 1, 0, 1> yyxy;};\
    struct{::legion::core::math::detail::swizzle<type, 2, 1, 1, 1, 0> yyyx;};\
