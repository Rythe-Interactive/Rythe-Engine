#include <physics/dataTemp/rigibody_data.hpp>
#include <core/events/event.hpp>
#include <core/events/eventbus.hpp>
#include <physics/events/events.hpp>

namespace legion::physics
{
    inline void rigidbodyData::sendModifyEvent()
    {
        //currently, when one parameter of the rigidbody is modified, we regard that change
        //as if all parameters were modified. This is done to avoid having one event for
        //each parameter (which would unnecessarily bloat the codebase).
        events::EventBus::raiseEvent<modifyRigidbody>(*this);
    }
}

