#include "component_handle.hpp"

void legion::core::ecs::component_handle_base::serialize(cereal::JSONOutputArchive& oarchive)
{
    oarchive(cereal::make_nvp("OWNER", m_ownerId));
}
void legion::core::ecs::component_handle_base::serialize(cereal::BinaryOutputArchive& oarchive)
{
    oarchive(cereal::make_nvp("OWNER", m_ownerId));
}
void legion::core::ecs::component_handle_base::serialize(cereal::JSONInputArchive& oarchive)
{
    oarchive(cereal::make_nvp("OWNER", m_ownerId));
}
void legion::core::ecs::component_handle_base::serialize(cereal::BinaryInputArchive& oarchive)
{
    oarchive(cereal::make_nvp("OWNER", m_ownerId));
}
