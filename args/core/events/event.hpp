#pragma once
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/platform/platform.hpp>

/**
 * @file event.hpp
 */

namespace args::core::events
{
	struct event_base
	{
		virtual id_type getId() ARGS_PURE;
		virtual bool persistent() ARGS_IMPURE_RETURN(false);
		virtual bool unique() ARGS_IMPURE_RETURN(true);

		virtual ~event_base() = default;
	};

	template<typename Self>
	struct event : public event_base
	{
		friend 	class EventBus;

		inline static const id_type id = typeHash<Self>();

		virtual ~event() = default;
	private:
		virtual id_type getId()
		{
			return id;
		}
	};
}