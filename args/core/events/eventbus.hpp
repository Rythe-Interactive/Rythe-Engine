#pragma once
#include <core/containers/delegate.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/types.hpp>
#include <core/events/event.hpp>

#include <memory>

/**@file eventbus.hpp
 */

namespace args::core::events
{
	class EventBus
	{
		sparse_map<id_type, hashed_sparse_set<event_base*>> m_events;
		sparse_map<id_type, multicast_delegate<void(EventBus*)>> m_eventCallbacks;

	public:
		~EventBus()
		{
			for(auto& events : m_events)
			{
				for (auto* event : events)
					delete event;

				events.clear();
			}
		}

		template<typename event_type, typename... Args, inherits_from<event_type, event<event_type>> = 0>
		void raiseEvent(Args... arguments)
		{
			if (!m_events.contains(event_type::id))
				m_events.emplace(event_type::id);

			m_events[event_type::id].insert(new event_type(arguments...));

			m_eventCallbacks[event_type::id].invoke(this);

			if (!m_events[event_type::id][0]->persistent())
			{
				for (auto* event : m_events[event_type::id])
					delete event;

				m_events[event_type::id].clear();
			}
		}

		template<typename event_type, inherits_from<event_type, event<event_type>> = 0>
		bool checkEvent() const
		{
			return m_events.contains(event_type::id) && m_events[event_type::id].size();
		}

		template<typename event_type, inherits_from<event_type, event<event_type>> = 0>
		size_type getEventCount() const
		{
			if (m_events.contains(event_type::id))
				return m_events[event_type::id].size();
			return 0;
		}

		template<typename event_type, inherits_from<event_type, event<event_type>> = 0>
		const event_type& getEvent(index_type index = 0) const
		{
			if (checkEvent<event_type>())
				return *static_cast<event_type*>(m_events[event_type::id][index]); // Static cast because we already know that the types are the same.
			return nullptr;
		}

		template<typename event_type, inherits_from<event_type, event<event_type>> = 0>
		const event_type& getLastEvent() const
		{
			if (checkEvent<event_type>())
			{
				size_type size = m_events[event_type::id].size();
				return *static_cast<event_type*>(m_events[event_type::id][size - 1]); // Static cast because we already know that the types are the same.
			}
			return nullptr;
		}

		template<typename event_type, inherits_from<event_type, event<event_type>> = 0>
		void clearEvent(index_type index = 0)
		{
			if (checkEvent<event_type>())
			{
				auto* event = m_events[event_type::id].dense()[index];
				m_events[event_type::id].erase(event);
				delete event;
			}
		}

		template<typename event_type, inherits_from<event_type, event<event_type>> = 0>
		void clearLastEvent()
		{
			if (checkEvent<event_type>())
			{
				auto* event = m_events[event_type::id].dense()[m_events[event_type::id].size() - 1];
				m_events[event_type::id].erase(event);
				delete event;
			}
		}

		template<typename event_type, inherits_from<event_type, event<event_type>> = 0>
		void bindToEvent(delegate<void(EventBus*)> callback)
		{
			m_eventCallbacks[typeid(event_type)] += callback;
		}
	};
}