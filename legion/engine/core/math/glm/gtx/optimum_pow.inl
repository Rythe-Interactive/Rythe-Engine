/// @ref gtx_optimum_pow

namespace legion::core::math
{
    template<typename genType>
    GLM_FUNC_QUALIFIER genType pow2(genType const& x)
    {
        return x * x;
    }

    template<typename genType>
    GLM_FUNC_QUALIFIER genType pow3(genType const& x)
    {
        return x * x * x;
    }

    template<typename genType>
    GLM_FUNC_QUALIFIER genType pow4(genType const& x)
    {
        return (x * x) * (x * x);
    }
}//namespace legion::core::math
