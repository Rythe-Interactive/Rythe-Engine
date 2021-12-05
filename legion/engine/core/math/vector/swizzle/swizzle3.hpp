#pragma once

#include <core/math/vector/swizzle/swizzle_base.hpp>

#define _MATH_SWIZZLE_3_2_(type)\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0> xx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1> yy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2> zz;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1> xy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0> yx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2> xz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0> zx;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2> yz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1> zy;};\

#define _MATH_SWIZZLE_3_3_(type)\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 0> xxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 1> yyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 2> zzz;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 1> xxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 0> xyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 1> xyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 0> yxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 1> yxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 0> yyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 2> xxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 0> xzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 2> xzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 0> zxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 2> zxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 0> zzx;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 2> yyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 1> yzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 2> yzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 1> zyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 2> zyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 1> zzy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 2> xyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 2> yxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 0> yzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 0> zyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 1> zxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 1> xzy;};\

#define _MATH_SWIZZLE_3_4_(type)\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 0, 0> xxxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 1, 1> yyyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 2, 2> zzzz;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 0, 1> xxxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 1, 0> xxyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 1, 1> xxyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 0, 0> xyxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 0, 1> xyxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 1, 0> xyyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 1, 1> xyyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 0, 0> yxxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 0, 1> yxxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 1, 0> yxyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 1, 1> yxyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 0, 0> yyxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 0, 1> yyxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 1, 0> yyyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 0, 2> xxxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 2, 0> xxzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 2, 2> xxzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 0, 0> xzxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 0, 2> xzxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 2, 0> xzzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 2, 2> xzzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 0, 0> zxxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 0, 2> zxxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 2, 0> zxzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 2, 2> zxzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 0, 0> zzxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 0, 2> zzxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 2, 0> zzzx;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 1, 2> yyyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 2, 1> yyzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 2, 2> yyzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 1, 1> yzyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 1, 2> yzyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 2, 1> yzzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 2, 2> yzzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 1, 1> zyyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 1, 2> zyyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 2, 1> zyzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 2, 2> zyzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 1, 1> zzyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 1, 2> zzyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 2, 1> zzzy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 2, 0> xyzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 0, 2> xyxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 1, 0> xzyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 0, 1> xzxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 1, 2> xxyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 2, 1> xxzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 2, 0> yxzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 0, 2> yxxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 0, 0> yzxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 0, 0> yzxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 0, 2> yxxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 2, 0> yxzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 1, 0> zxyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 0, 1> zxxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 0, 0> zyxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 0, 0> zyxx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 0, 1> zxxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 1, 0> zxyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 1, 2> xxyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 0, 2, 1> xxzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 0, 2> xyxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 2, 0> xyzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 0, 1> xzxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 1, 0> xzyx;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 2, 1> xyzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 1, 2> xyyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 1, 1> xzyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 1, 1> xzyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 1, 2> xyyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 2, 1> xyzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 2, 1> yxzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 1, 2> yxyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 0, 1> yzxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 1, 0> yzyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 0, 2> yyxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 2, 0> yyzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 1, 1> zxyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 1, 1> zxyy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 0, 1> zyxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 1, 0> zyyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 0, 1> zyxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 1, 0> zyyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 1, 2> yxyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 2, 1> yxzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 0, 2> yyxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 1, 2, 0> yyzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 0, 1> yzxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 1, 0> yzyx;};\
\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 2, 2> xyzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 1, 2, 2> xyzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 1, 2> xzyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 2, 1> xzzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 1, 2> xzyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 0, 2, 2, 1> xzzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 2, 2> yxzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 0, 2, 2> yxzz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 0, 2> yzxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 2, 0> yzzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 0, 2> yzxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 1, 2, 2, 0> yzzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 1, 2> zxyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 2, 1> zxzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 0, 2> zyxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 2, 0> zyzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 0, 1> zzxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 1, 0> zzyx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 1, 2> zxyz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 0, 2, 1> zxzy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 0, 2> zyxz;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 1, 2, 0> zyzx;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 0, 1> zzxy;};\
    struct{::legion::core::math::detail::swizzle<type, 3, 2, 2, 1, 0> zzyx;};\
