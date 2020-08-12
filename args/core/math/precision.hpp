#pragma once
#include <core/types/primitives.hpp>


namespace args::core::maths
{
	enum class precision
	{
		bit32,
		bit64,
		lots
	};


	template <precision p>
	struct precision_chooser
	{
		/*if*/		std::conditional_t<p == precision::bit32,f32,
		/*elseif*/	std::conditional_t<p == precision::bit64,f64,
		/*else*/	f80>> type;
	};

	template <precision p>
	using precision_chooser_t = typename precision_chooser<p>::type;

	
}