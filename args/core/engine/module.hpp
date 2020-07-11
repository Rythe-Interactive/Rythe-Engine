#pragma once
#include <core/types/primitives.hpp>

namespace args::core
{
	class Module
	{
	public:
		virtual priority_type priority() = 0;
		virtual void init() = 0;
	};
}
