#include "../detail/_vectorize.hpp"

namespace legion::core::math
{
    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<L, T, Q> min(vec<L, T, Q> const& x, vec<L, T, Q> const& y, vec<L, T, Q> const& z)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'min' only accept floating-point or integer inputs");
        return math::min(math::min(x, y), z);
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<L, T, Q> min(vec<L, T, Q> const& x, vec<L, T, Q> const& y, vec<L, T, Q> const& z, vec<L, T, Q> const& w)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'min' only accept floating-point or integer inputs");
        return math::min(math::min(x, y), math::min(z, w));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<L, T, Q> max(vec<L, T, Q> const& x, vec<L, T, Q> const& y, vec<L, T, Q> const& z)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'max' only accept floating-point or integer inputs");
        return math::max(math::max(x, y), z);
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER GLM_CONSTEXPR vec<L, T, Q> max(vec<L, T, Q> const& x, vec<L, T, Q> const& y, vec<L, T, Q> const& z, vec<L, T, Q> const& w)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'max' only accept floating-point or integer inputs");
        return math::max(math::max(x, y), math::max(z, w));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fmin(vec<L, T, Q> const& a, T b)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fmin' only accept floating-point inputs");
        return detail::functor2<vec, L, T, Q>::call(fmin, a, vec<L, T, Q>(b));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fmin(vec<L, T, Q> const& a, vec<L, T, Q> const& b)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fmin' only accept floating-point inputs");
        return detail::functor2<vec, L, T, Q>::call(fmin, a, b);
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fmin(vec<L, T, Q> const& a, vec<L, T, Q> const& b, vec<L, T, Q> const& c)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fmin' only accept floating-point inputs");
        return fmin(fmin(a, b), c);
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fmin(vec<L, T, Q> const& a, vec<L, T, Q> const& b, vec<L, T, Q> const& c, vec<L, T, Q> const& d)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fmin' only accept floating-point inputs");
        return fmin(fmin(a, b), fmin(c, d));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fmax(vec<L, T, Q> const& a, T b)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fmax' only accept floating-point inputs");
        return detail::functor2<vec, L, T, Q>::call(fmax, a, vec<L, T, Q>(b));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fmax(vec<L, T, Q> const& a, vec<L, T, Q> const& b)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fmax' only accept floating-point inputs");
        return detail::functor2<vec, L, T, Q>::call(fmax, a, b);
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fmax(vec<L, T, Q> const& a, vec<L, T, Q> const& b, vec<L, T, Q> const& c)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fmax' only accept floating-point inputs");
        return fmax(fmax(a, b), c);
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fmax(vec<L, T, Q> const& a, vec<L, T, Q> const& b, vec<L, T, Q> const& c, vec<L, T, Q> const& d)
    {
        GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'fmax' only accept floating-point inputs");
        return fmax(fmax(a, b), fmax(c, d));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fclamp(vec<L, T, Q> const& x, T minVal, T maxVal)
    {
        return fmin(fmax(x, vec<L, T, Q>(minVal)), vec<L, T, Q>(maxVal));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> fclamp(vec<L, T, Q> const& x, vec<L, T, Q> const& minVal, vec<L, T, Q> const& maxVal)
    {
        return fmin(fmax(x, minVal), maxVal);
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> clamp(vec<L, T, Q> const& Texcoord)
    {
        return math::clamp(Texcoord, vec<L, T, Q>(0), vec<L, T, Q>(1));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> repeat(vec<L, T, Q> const& Texcoord)
    {
        return math::fract(Texcoord);
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> mirrorClamp(vec<L, T, Q> const& Texcoord)
    {
        return math::fract(math::abs(Texcoord));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER vec<L, T, Q> mirrorRepeat(vec<L, T, Q> const& Texcoord)
    {
        vec<L, T, Q> const Abs = math::abs(Texcoord);
        vec<L, T, Q> const Clamp = math::mod(math::floor(Abs), vec<L, T, Q>(2));
        vec<L, T, Q> const Floor = math::floor(Abs);
        vec<L, T, Q> const Rest = Abs - Floor;
        vec<L, T, Q> const Mirror = Clamp + Rest;
        return mix(Rest, vec<L, T, Q>(1) - Rest, math::greaterThanEqual(Mirror, vec<L, T, Q>(1)));
    }
}//namespace legion::core::math
