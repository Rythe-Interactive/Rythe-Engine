#pragma once
#include <core/containers/delegate.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/types/types.hpp>
#include <core/events/event.hpp>

#include <memory>

/**@file eventbus.hpp
 */

namespace args::core::events
{
	class EventBus
	{
		sparse_map<id_type, std::unique_ptr<event_base>> m_events;
		sparse_map<id_type,> m_events;

	public:
		template<typename event_type, typename... Args, inherits_from<event_type, event_base> = 0>
		inline void raiseEvent(Args... arguments)
		{
			m_events.insert(event_type::id, std::make_unique<event_type>(arguments...));

			for (auto callback : eventCallbacks[event_type::id])
				callback(m_events[event_type::id].get());
		}

		template<typename EventType, typename>
		inline bool checkEvent()
		{
			return m_events.contains(EventType::id);
		}

		template<typename EventType, typename>
		inline EventType* getEvent()
		{
			if (checkEvent<EventType>())
				return static_cast<EventType*>(m_events[EventType::id].get());
			return nullptr;
		}

		template<typename EventType, typename>
		inline void clearEvent()
		{
			if (checkEvent<EventType>())
			{
				m_events.erase(EventType::id);
			}
		}

		template<typename EventType, typename>
		inline void bindToEvent(std::function<void(Args::IEvent*)> callback)
		{
			eventCallbacks[typeid(EventType)].push_back(callback);
		}
	};
}