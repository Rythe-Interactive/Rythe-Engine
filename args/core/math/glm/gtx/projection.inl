/// @ref gtx_projection

namespace args::core::math::detail::glm
{
	template<typename genType>
	GLM_FUNC_QUALIFIER genType proj(genType const& x, genType const& Normal)
	{
		return glm::dot(x, Normal) / glm::dot(Normal, Normal) * Normal;
	}
}//namespace args::core::math::detail::glm
