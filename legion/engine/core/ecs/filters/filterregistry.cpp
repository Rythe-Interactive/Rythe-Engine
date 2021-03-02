#include <core/ecs/filters/filterregistry.hpp>

namespace legion::core::ecs
{
    void FilterRegistry::markEntityDestruction(entity target)
    {
        for (auto& [_, entityList] : m_entityLists)
            entityList.erase(target); // Will not do anything if the target wasn't in the set.
    }

}
