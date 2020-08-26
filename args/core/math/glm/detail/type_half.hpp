#pragma once

#include "setup.hpp"

namespace args::core::math::detail::glm{
namespace detail
{
	typedef short hdata;

	GLM_FUNC_DECL float toFloat32(hdata value);
	GLM_FUNC_DECL hdata toFloat16(float const& value);

}//namespace detail
}//namespace args::core::math::detail::glm

#include "type_half.inl"
