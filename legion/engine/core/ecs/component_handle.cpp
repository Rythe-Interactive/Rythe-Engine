#include "component_handle.hpp"

namespace legion::core::ecs
{
    EcsRegistry* component_handle_base::m_registry = nullptr;

    void legion::core::ecs::component_handle_base::serialize(cereal::JSONOutputArchive& oarchive)
    {
        oarchive(cereal::make_nvp("Owner", m_ownerId), cereal::make_nvp("Component Type", m_typeId));
        m_registry->getFamily(m_typeId)->serialize(oarchive, m_ownerId);
    }
    void legion::core::ecs::component_handle_base::serialize(cereal::BinaryOutputArchive& oarchive)
    {
        oarchive(cereal::make_nvp("Owner", m_ownerId), cereal::make_nvp("Component Type", m_typeId));
        m_registry->getFamily(m_typeId)->serialize(oarchive, m_ownerId);

    }
    void legion::core::ecs::component_handle_base::serialize(cereal::JSONInputArchive& oarchive)
    {
        oarchive(cereal::make_nvp("Owner", m_ownerId), cereal::make_nvp("Component Type", m_typeId));
        m_registry->createComponent(m_ownerId, m_typeId);
        m_registry->getFamily(m_typeId)->serialize(oarchive, m_ownerId);

    }
    void legion::core::ecs::component_handle_base::serialize(cereal::BinaryInputArchive& oarchive)
    {
        oarchive(cereal::make_nvp("Owner", m_ownerId), cereal::make_nvp("Component Type", m_typeId));
        m_registry->createComponent(m_ownerId, m_typeId);
        m_registry->getFamily(m_typeId)->serialize(oarchive, m_ownerId);
    }
}
