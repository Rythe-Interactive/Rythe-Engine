/// @ref gtx_projection

namespace legion::core::math
{
    template<typename genType>
    GLM_FUNC_QUALIFIER genType proj(genType const& x, genType const& Normal)
    {
        return math::dot(x, Normal) / math::dot(Normal, Normal) * Normal;
    }
}//namespace legion::core::math
