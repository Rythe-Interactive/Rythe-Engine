#pragma once
#include <core/engine/system.hpp>
#include <core/defaults/defaultcomponents.hpp>

namespace legion::core
{
    class HierarchySystem : public System<HierarchySystem>
    {
    public:
        void onPositionModified(events::component_modification<position>* event);
        void onRotationModified(events::component_modification<rotation>* event);
        void onScaleModified(events::component_modification<scale>* event);

        virtual void setup();
    };
}
