#pragma once

#include <core/math/vector/swizzle/swizzle_base.hpp>

#define _MATH_SWIZZLE_2_1_(type)\
    struct{::legion::core::math::swizzle<type, 1, 0> x;};\
    struct{::legion::core::math::swizzle<type, 1, 0> u;};\
    struct{::legion::core::math::swizzle<type, 1, 0> r;};\
    struct{::legion::core::math::swizzle<type, 1, 0> s;};\
\
    struct{::legion::core::math::swizzle<type, 1, 1> y;};\
    struct{::legion::core::math::swizzle<type, 1, 1> v;};\
    struct{::legion::core::math::swizzle<type, 1, 1> g;};\
    struct{::legion::core::math::swizzle<type, 1, 1> t;};\

#define _MATH_SWIZZLE_2_2_(type)\
    struct{::legion::core::math::swizzle<type, 2, 0, 0> xx;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1> yy;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 1> xy;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0> yx;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0> rr;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1> gg;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 1> rg;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0> gr;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0> uu;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1> vv;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 1> uv;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0> vu;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0> ss;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1> tt;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 1> st;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0> ts;};\

#define _MATH_SWIZZLE_2_3_(type)\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0> xxx;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1> yyy;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1> xxy;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0> xyx;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1> xyy;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0> yxx;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1> yxy;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0> yyx;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0> rrr;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1> ggg;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1> rrg;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0> rgr;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1> rgg;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0> grr;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1> grg;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0> ggr;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0> uuu;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1> vvv;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1> uuv;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0> uvu;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1> uvv;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0> vuu;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1> vuv;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0> vvu;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0> sss;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1> ttt;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1> sst;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0> sts;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1> stt;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0> tss;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1> tst;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0> tts;};\

#define _MATH_SWIZZLE_2_4_(type)\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0, 0> xxxx;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1, 1> yyyy;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0, 1> xxxy;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1, 0> xxyx;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1, 1> xxyy;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0, 0> xyxx;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0, 1> xyxy;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1, 0> xyyx;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1, 1> xyyy;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0, 0> yxxx;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0, 1> yxxy;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1, 0> yxyx;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1, 1> yxyy;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0, 0> yyxx;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0, 1> yyxy;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1, 0> yyyx;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0, 0> rrrr;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1, 1> gggg;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0, 1> rrrg;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1, 0> rrgr;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1, 1> rrgg;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0, 0> rgrr;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0, 1> rgrg;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1, 0> rggr;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1, 1> rggg;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0, 0> grrr;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0, 1> grrg;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1, 0> grgr;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1, 1> grgg;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0, 0> ggrr;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0, 1> ggrg;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1, 0> gggr;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0, 0> uuuu;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1, 1> vvvv;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0, 1> uuuv;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1, 0> uuvu;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1, 1> uuvv;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0, 0> uvuu;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0, 1> uvuv;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1, 0> uvvu;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1, 1> uvvv;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0, 0> vuuu;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0, 1> vuuv;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1, 0> vuvu;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1, 1> vuvv;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0, 0> vvuu;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0, 1> vvuv;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1, 0> vvvu;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0, 0> ssss;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1, 1> tttt;};\
\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 0, 1> ssst;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1, 0> ssts;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 0, 1, 1> sstt;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0, 0> stss;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 0, 1> stst;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1, 0> stts;};\
    struct{::legion::core::math::swizzle<type, 2, 0, 1, 1, 1> sttt;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0, 0> tsss;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 0, 1> tsst;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1, 0> tsts;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 0, 1, 1> tstt;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0, 0> ttss;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 0, 1> ttst;};\
    struct{::legion::core::math::swizzle<type, 2, 1, 1, 1, 0> ttts;};\
