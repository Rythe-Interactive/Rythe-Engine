/// @ref gtx_projection

namespace args::core::math
{
	template<typename genType>
	GLM_FUNC_QUALIFIER genType proj(genType const& x, genType const& Normal)
	{
		return math::dot(x, Normal) / math::dot(Normal, Normal) * Normal;
	}
}//namespace args::core::math
