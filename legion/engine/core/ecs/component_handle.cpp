#include "component_handle.hpp"

namespace legion::core::ecs
{
    EcsRegistry* component_handle_base::m_registry = nullptr;
    events::EventBus* component_handle_base::m_eventBus = nullptr;

    void legion::core::ecs::component_handle_base::serialize(cereal::JSONOutputArchive& oarchive)
    {
        OPTICK_EVENT();
        id_type ownerId = entity.get_id();
        oarchive(cereal::make_nvp("Owner", ownerId), cereal::make_nvp("Component Type", m_typeId));
        m_registry->getFamily(m_typeId)->serialize(oarchive, ownerId);
    }
    void legion::core::ecs::component_handle_base::serialize(cereal::BinaryOutputArchive& oarchive)
    {
        OPTICK_EVENT();
        id_type ownerId = entity.get_id();
        oarchive(cereal::make_nvp("Owner", ownerId), cereal::make_nvp("Component Type", m_typeId));
        m_registry->getFamily(m_typeId)->serialize(oarchive, ownerId);

    }
    void legion::core::ecs::component_handle_base::serialize(cereal::JSONInputArchive& oarchive)
    {
        OPTICK_EVENT();
        id_type ownerId = entity.get_id();
        oarchive(cereal::make_nvp("Owner", ownerId), cereal::make_nvp("Component Type", m_typeId));
        m_registry->createComponent(ownerId, m_typeId);
        m_registry->getFamily(m_typeId)->serialize(oarchive, ownerId);

    }
    void legion::core::ecs::component_handle_base::serialize(cereal::BinaryInputArchive& oarchive)
    {
        OPTICK_EVENT();
        id_type ownerId = entity.get_id();
        oarchive(cereal::make_nvp("Owner", ownerId), cereal::make_nvp("Component Type", m_typeId));
        m_registry->createComponent(ownerId, m_typeId);
        m_registry->getFamily(m_typeId)->serialize(oarchive, ownerId);
    }
}
