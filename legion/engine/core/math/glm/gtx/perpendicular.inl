/// @ref gtx_perpendicular

namespace legion::core::math
{
    template<typename genType>
    GLM_FUNC_QUALIFIER genType perp(genType const& x, genType const& Normal)
    {
        return x - proj(x, Normal);
    }
}//namespace legion::core::math
