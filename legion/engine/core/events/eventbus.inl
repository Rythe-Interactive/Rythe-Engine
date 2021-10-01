#include <core/events/eventbus.hpp>
#pragma once

namespace legion::core::events
{
    template<typename event_type, typename... Args CNDOXY(typename)>
    inline void EventBus::raiseEvent(Args&&... arguments)
    {
        if (instance.m_eventCallbacks.count(event_type::id))
        {
            event_type event(arguments...); // Create new event.
            force_value_cast<multicast_delegate<void(event_type&)>>(instance.m_eventCallbacks.at(event_type::id)).invoke(event); // Notify.
        }
    }

    template<typename event_type CNDOXY(typename)>
    inline L_ALWAYS_INLINE void EventBus::bindToEvent(const delegate<void(event_type&)>& callback)
    {
        instance.m_eventCallbacks.try_emplace(event_type::id).first->second.push_back(reinterpret_cast<const delegate<void(event_base&)>&>(callback));
    }

    template<typename event_type CNDOXY(typename)>
    inline L_ALWAYS_INLINE void EventBus::bindToEvent(delegate<void(event_type&)>&& callback)
    {
        instance.m_eventCallbacks.try_emplace(event_type::id).first->second.push_back(reinterpret_cast<delegate<void(event_base&)>&&>(callback));
    }

    template<typename event_type CNDOXY(typename)>
    inline L_ALWAYS_INLINE void EventBus::unbindFromEvent(const delegate<void(event_type&)>& callback)
    {
        instance.m_eventCallbacks.at(event_type::id).erase(reinterpret_cast<const delegate<void(event_base&)>&>(callback));
    }

}
