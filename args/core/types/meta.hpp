#pragma once
#include <type_traits>

namespace args::core
{
	template<typename derived_type, typename base_type>
	using inherits_from = typename std::enable_if<std::is_base_of<base_type, derived_type>::value, int>::type;
}