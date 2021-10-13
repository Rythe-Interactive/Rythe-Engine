/// @ref gtx_normalize_dot

namespace legion::core::math
{
    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER T normalizeDot(vec<L, T, Q> const& x, vec<L, T, Q> const& y)
    {
        return math::dot(x, y) * math::inversesqrt(math::dot(x, x) * math::dot(y, y));
    }

    template<length_t L, typename T, qualifier Q>
    GLM_FUNC_QUALIFIER T fastNormalizeDot(vec<L, T, Q> const& x, vec<L, T, Q> const& y)
    {
        return math::dot(x, y) * math::fastInverseSqrt(math::dot(x, x) * math::dot(y, y));
    }
}//namespace legion::core::math
