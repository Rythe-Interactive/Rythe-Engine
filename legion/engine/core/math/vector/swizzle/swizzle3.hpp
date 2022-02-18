#pragma once

#include <core/math/vector/swizzle/swizzle_base.hpp>

#define _MATH_SWIZZLE_3_1_(type)\
    struct{::legion::core::math::swizzle<type, 1, 0> x;};\
    struct{::legion::core::math::swizzle<type, 1, 0> u;};\
    struct{::legion::core::math::swizzle<type, 1, 0> r;};\
    struct{::legion::core::math::swizzle<type, 1, 0> s;};\
\
    struct{::legion::core::math::swizzle<type, 1, 1> y;};\
    struct{::legion::core::math::swizzle<type, 1, 1> v;};\
    struct{::legion::core::math::swizzle<type, 1, 1> g;};\
    struct{::legion::core::math::swizzle<type, 1, 1> t;};\
\
    struct{::legion::core::math::swizzle<type, 1, 2> z;};\
    struct{::legion::core::math::swizzle<type, 1, 2> w;};\
    struct{::legion::core::math::swizzle<type, 1, 2> b;};\
    struct{::legion::core::math::swizzle<type, 1, 2> p;};\

#define _MATH_SWIZZLE_3_2_(type)\
    struct{::legion::core::math::swizzle<type, 3, 0, 0> xx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1> yy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2> zz;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1> xy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0> yx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2> xz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0> zx;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 2> yz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1> zy;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0> rr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1> gg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2> bb;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1> rg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0> gr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2> rb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0> br;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 2> gb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1> bg;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0> uu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1> vv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2> ww;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1> uv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0> vu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2> uw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0> wu;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 2> vw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1> wv;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0> ss;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1> tt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2> pp;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1> st;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0> ts;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2> sp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0> ps;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 2> tp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1> pt;};\

#define _MATH_SWIZZLE_3_3_(type)\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0> xxx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1> yyy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2> zzz;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1> xxy;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0> xyx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1> xyy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0> yxx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1> yxy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0> yyx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2> xxz;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0> xzx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2> xzz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0> zxx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2> zxz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0> zzx;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2> yyz;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1> yzy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2> yzz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1> zyy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2> zyz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1> zzy;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2> xyz;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2> yxz;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0> yzx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0> zyx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1> zxy;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1> xzy;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0> rrr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1> ggg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2> bbb;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1> rrg;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0> rgr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1> rgg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0> grr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1> grg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0> ggr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2> rrb;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0> rbr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2> rbb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0> brr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2> brb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0> bbr;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2> ggb;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1> gbg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2> gbb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1> bgg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2> bgb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1> bbg;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2> rgb;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2> grb;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0> gbr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0> bgr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1> brg;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1> rbg;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0> uuu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1> vvv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2> www;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1> uuv;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0> uvu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1> uvv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0> vuu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1> vuv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0> vvu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2> uuw;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0> uwu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2> uww;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0> wuu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2> wuw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0> wwu;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2> vvw;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1> vwv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2> vww;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1> wvv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2> wvw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1> wwv;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2> uvw;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2> vuw;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0> vwu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0> wvu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1> wuv;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1> uwv;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0> sss;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1> ttt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2> ppp;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1> sst;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0> sts;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1> stt;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0> tss;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1> tst;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0> tts;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2> ssp;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0> sps;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2> spp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0> pss;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2> psp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0> pps;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2> ttp;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1> tpt;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2> tpp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1> ptt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2> ptp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1> ppt;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2> stp;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2> tsp;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0> tps;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0> pts;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1> pst;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1> spt;};\

#define _MATH_SWIZZLE_3_4_(type)\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 0> xxxx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 1> yyyy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 2> zzzz;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 1> xxxy;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 0> xxyx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 1> xxyy;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 0> xyxx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 1> xyxy;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 0> xyyx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 1> xyyy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 0> yxxx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 1> yxxy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 0> yxyx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 1> yxyy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 0> yyxx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 1> yyxy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 0> yyyx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 2> xxxz;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 0> xxzx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 2> xxzz;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 0> xzxx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 2> xzxz;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 0> xzzx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 2> xzzz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 0> zxxx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 2> zxxz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 0> zxzx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 2> zxzz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 0> zzxx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 2> zzxz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 0> zzzx;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 2> yyyz;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 1> yyzy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 2> yyzz;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 1> yzyy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 2> yzyz;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 1> yzzy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 2> yzzz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 1> zyyy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 2> zyyz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 1> zyzy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 2> zyzz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 1> zzyy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 2> zzyz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 1> zzzy;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 0> xyzx;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 2> xyxz;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 2> xxyz;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 1> xxzy;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 1> xzxy;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 0> xzyx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 0> yzxx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 0> yxzx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 2> yxxz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 0> zyxx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 0> zxyx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 1> zxxy;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 1> xyzy;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 2> xyyz;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 1> xzyy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 1> yzxy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 0> yzyx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 0> yyzx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 2> yyxz;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 2> yxyz;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 1> yxzy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 1> zyxy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 1> zxyy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 0> zyyx;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 2> xyzz;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 2> xzyz;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 1> xzzy;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 0> yzzx;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 2> yzxz;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 2> yxzz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 1> zxzy;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 2> zxyz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 2> zyxz;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 0> zyzx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 0> zzyx;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 1> zzxy;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 0> rrrr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 1> gggg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 2> bbbb;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 1> rrrg;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 0> rrgr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 1> rrgg;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 0> rgrr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 1> rgrg;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 0> rggr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 1> rggg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 0> grrr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 1> grrg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 0> grgr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 1> grgg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 0> ggrr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 1> ggrg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 0> gggr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 2> rrrb;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 0> rrbr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 2> rrbb;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 0> rbrr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 2> rbrb;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 0> rbbr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 2> rbbb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 0> brrr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 2> brrb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 0> brbr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 2> brbb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 0> bbrr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 2> bbrb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 0> bbbr;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 2> gggb;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 1> ggbg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 2> ggbb;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 1> gbgg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 2> gbgb;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 1> gbbg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 2> gbbb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 1> bggg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 2> bggb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 1> bgbg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 2> bgbb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 1> bbgg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 2> bbgb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 1> bbbg;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 0> rgbr;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 2> rgrb;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 2> rrgb;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 1> rrbg;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 1> rbrg;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 0> rbgr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 0> gbrr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 0> grbr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 2> grrb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 0> bgrr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 0> brgr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 1> brrg;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 1> rgbg;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 2> rggb;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 1> rbgg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 1> gbrg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 0> gbgr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 0> ggbr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 2> ggrb;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 2> grgb;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 1> grbg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 1> bgrg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 1> brgg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 0> bggr;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 2> rgbb;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 2> rbgb;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 1> rbbg;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 0> gbbr;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 2> gbrb;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 2> grbb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 1> brbg;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 2> brgb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 2> bgrb;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 0> bgbr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 0> bbgr;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 1> bbrg;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 0> uuuu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 1> vvvv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 2> wwww;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 1> uuuv;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 0> uuvu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 1> uuvv;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 0> uvuu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 1> uvuv;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 0> uvvu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 1> uvvv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 0> vuuu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 1> vuuv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 0> vuvu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 1> vuvv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 0> vvuu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 1> vvuv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 0> vvvu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 2> uuuw;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 0> uuwu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 2> uuww;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 0> uwuu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 2> uwuw;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 0> uwwu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 2> uwww;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 0> wuuu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 2> wuuw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 0> wuwu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 2> wuww;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 0> wwuu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 2> wwuw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 0> wwwu;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 2> vvvw;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 1> vvwv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 2> vvww;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 1> vwvv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 2> vwvw;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 1> vwwv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 2> vwww;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 1> wvvv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 2> wvvw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 1> wvwv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 2> wvww;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 1> wwvv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 2> wwvw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 1> wwwv;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 0> uvwu;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 2> uvuw;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 2> uuvw;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 1> uuwv;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 1> uwuv;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 0> uwvu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 0> vwuu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 0> vuwu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 2> vuuw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 0> wvuu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 0> wuvu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 1> wuuv;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 1> uvwv;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 2> uvvw;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 1> uwvv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 1> vwuv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 0> vwvu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 0> vvwu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 2> vvuw;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 2> vuvw;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 1> vuwv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 1> wvuv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 1> wuvv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 0> wvvu;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 2> uvww;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 2> uwvw;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 1> uwwv;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 0> vwwu;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 2> vwuw;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 2> vuww;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 1> wuwv;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 2> wuvw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 2> wvuw;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 0> wvwu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 0> wwvu;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 1> wwuv;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 0> ssss;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 1> tttt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 2> pppp;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 1> ssst;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 0> ssts;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 1> sstt;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 0> stss;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 1> stst;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 0> stts;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 1> sttt;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 0> tsss;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 1> tsst;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 0> tsts;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 1> tstt;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 0> ttss;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 1> ttst;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 0> ttts;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 0, 2> sssp;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 0> ssps;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 2> sspp;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 0> spss;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 2> spsp;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 0> spps;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 2> sppp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 0> psss;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 2> pssp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 0> psps;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 2> pspp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 0> ppss;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 2> ppsp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 0> ppps;};\
\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 1, 2> tttp;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 1> ttpt;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 2> ttpp;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 1> tptt;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 2> tptp;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 1> tppt;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 2> tppp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 1> pttt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 2> pttp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 1> ptpt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 2> ptpp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 1> pptt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 2> pptp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 2, 1> pppt;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 0> stps;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 0, 2> stsp;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 1, 2> sstp;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 0, 2, 1> sspt;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 0, 1> spst;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 0> spts;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 0> tpss;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 0> tsps;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 0, 2> tssp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 0> ptss;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 0> psts;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 0, 1> psst;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 1> stpt;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 1, 2> sttp;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 1> sptt;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 1> tpst;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 1, 0> tpts;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 2, 0> ttps;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 1, 0, 2> ttsp;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 1, 2> tstp;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 1> tspt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 1> ptst;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 1> pstt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 1, 0> ptts;};\
\
    struct{::legion::core::math::swizzle<type, 3, 0, 1, 2, 2> stpp;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 1, 2> sptp;};\
    struct{::legion::core::math::swizzle<type, 3, 0, 2, 2, 1> sppt;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 2, 0> tpps;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 2, 0, 2> tpsp;};\
    struct{::legion::core::math::swizzle<type, 3, 1, 0, 2, 2> tspp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 2, 1> pspt;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 0, 1, 2> pstp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 0, 2> ptsp;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 1, 2, 0> ptps;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 1, 0> ppts;};\
    struct{::legion::core::math::swizzle<type, 3, 2, 2, 0, 1> ppst;};\
