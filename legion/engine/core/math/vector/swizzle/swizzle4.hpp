#pragma once

#include <core/math/vector/swizzle/swizzle_base.hpp>

#define _MATH_SWIZZLE_4_2_(type)\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0> xx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1> yy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2> zz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3> ww;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1> xy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0> yx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2> xz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0> zx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3> xw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0> wx;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2> yz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1> zy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3> yw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1> wy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3> zw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2> wz;};\

#define _MATH_SWIZZLE_4_3_(type)\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 0> xxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 1> yyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 2> zzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 3> www;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 1> xxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 0> xyx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 1> xyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 0> yxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 1> yxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 0> yyx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 2> xxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 0> xzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 2> xzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 0> zxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 2> zxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 0> zzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 3> xxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 0> xwx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 3> xww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 0> wxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 3> wxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 0> wwx;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 2> yyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 1> yzy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 2> yzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 1> zyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 2> zyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 1> zzy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 3> yyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 1> ywy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 3> yww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 1> wyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 3> wyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 1> wwy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 3> zzw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 2> zwz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 3> zww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 2> wzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 3> wzw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 2> wwz;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 2> xyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 2> yxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 0> yzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 0> zyx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 1> zxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 1> xzy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 2> wyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 2> ywz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 3> yzw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 3> zyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 1> zwy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 1> wzy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 2> xwz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 2> wxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 0> wzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 0> zwx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 3> zxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 3> xzw;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 3> xyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 3> yxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 0> ywx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 0> wyx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 1> wxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 1> xwy;};\

#define _MATH_SWIZZLE_4_4_(type)\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 0, 0> xxxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 1, 1> yyyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 2, 2> zzzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 3, 3> wwww;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 0, 1> xxxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 1, 0> xxyx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 1, 1> xxyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 0, 0> xyxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 0, 1> xyxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 1, 0> xyyx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 1, 1> xyyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 0, 0> yxxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 0, 1> yxxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 1, 0> yxyx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 1, 1> yxyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 0, 0> yyxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 0, 1> yyxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 1, 0> yyyx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 0, 2> xxxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 2, 0> xxzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 2, 2> xxzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 0, 0> xzxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 0, 2> xzxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 2, 0> xzzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 2, 2> xzzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 0, 0> zxxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 0, 2> zxxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 2, 0> zxzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 2, 2> zxzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 0, 0> zzxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 0, 2> zzxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 2, 0> zzzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 0, 3> xxxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 3, 0> xxwx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 0, 3, 3> xxww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 0, 0> xwxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 0, 3> xwxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 3, 0> xwwx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 3, 3> xwww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 0, 0> wxxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 0, 3> wxxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 3, 0> wxwx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 3, 3> wxww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 0, 0> wwxx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 0, 3> wwxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 3, 0> wwwx;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 1, 2> yyyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 2, 1> yyzy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 2, 2> yyzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 1, 1> yzyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 1, 2> yzyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 2, 1> yzzy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 2, 2> yzzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 1, 1> zyyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 1, 2> zyyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 2, 1> zyzy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 2, 2> zyzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 1, 1> zzyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 1, 2> zzyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 2, 1> zzzy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 1, 3> yyyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 3, 1> yywy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 1, 3, 3> yyww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 1, 1> ywyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 1, 3> ywyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 3, 1> ywwy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 3, 3> ywww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 1, 1> wyyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 1, 3> wyyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 3, 1> wywy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 3, 3> wyww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 1, 1> wwyy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 1, 3> wwyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 3, 1> wwwy;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 2, 3> zzzw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 3, 2> zzwz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 2, 3, 3> zzww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 2, 2> zwzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 2, 3> zwzw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 3, 2> zwwz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 3, 3> zwww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 2, 2> wzzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 2, 3> wzzw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 3, 2> wzwz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 3, 3> wzww;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 2, 2> wwzz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 2, 3> wwzw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 3, 3, 2> wwwz;};\
\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 2, 3> xyzw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 1, 3, 2> xywz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 1, 3> xzyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 2, 3, 1> xzwy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 1, 2> xwyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 0, 3, 2, 1> xwzy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 2, 3> yxzw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 0, 3, 2> yxwz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 0, 3> yzxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 2, 3, 0> yzwx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 0, 2> ywxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 1, 3, 2, 0> ywzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 1, 3> zxyw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 0, 3, 1> zxwy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 0, 3> zyxw;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 1, 3, 0> zywx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 0, 1> zwxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 2, 3, 1, 0> zwyx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 1, 2> wxyz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 0, 2, 1> wxzy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 0, 2> wyxz;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 1, 2, 0> wyzx;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 0, 1> wzxy;};\
    struct{::legion::core::math::detail::swizzle<type, 4, 3, 2, 1, 0> wzyx;};\
