#pragma once
#include <core/types/primitives.hpp>


namespace legion::core::math
{
	enum class data_precision
	{
		bit32,
		bit64,
		lots,
		whole_only
	};


	template <data_precision p>
	struct precision_chooser
	{
		using type = 
		/*if*/		std::conditional_t<p == data_precision::bit32,f32,
		/*elseif*/	std::conditional_t<p == data_precision::bit64,f64,
		/*elseif*/	std::conditional_t<p == data_precision::lots, f80,
					int32>>>;
	};

	template <data_precision p>
	using precision_chooser_t = typename precision_chooser<p>::type;
		
}
