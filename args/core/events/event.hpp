#pragma once
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>

/**
 * @file event.hpp
 */

namespace args::core::events
{
	struct event_base
	{
	public:
		virtual id_type getId() = 0;

	};

	template<typename Self>
	struct event : public event_base
	{
		inline static const id_type id = typeHash<Self>();

		virtual id_type getId()
		{
			return id;
		}
	};
}