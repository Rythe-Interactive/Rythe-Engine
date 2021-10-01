#include <core/events/eventbus.hpp>

namespace legion::core::events
{
    void EventBus::raiseEvent(event_base& value)
    {
        if (instance.m_eventCallbacks.count(value.get_id()))
            instance.m_eventCallbacks.at(value.get_id()).invoke(value);
    }

    void EventBus::raiseEventUnsafe(event_base& value, id_type id)
    {
        if (instance.m_eventCallbacks.count(id))
            instance.m_eventCallbacks.at(id).invoke(value);
    }

    void EventBus::bindToEvent(id_type id, const delegate<void(event_base&)>& callback)
    {
        instance.m_eventCallbacks.try_emplace(id).first->second.push_back(callback);
    }

    void EventBus::bindToEvent(id_type id, delegate<void(event_base&)>&& callback)
    {
        instance.m_eventCallbacks.try_emplace(id).first->second.push_back(std::move(callback));
    }

    void EventBus::unbindFromEvent(id_type id, const delegate<void(event_base&)>& callback)
    {
        instance.m_eventCallbacks.at(id).erase(callback);
    }

}
