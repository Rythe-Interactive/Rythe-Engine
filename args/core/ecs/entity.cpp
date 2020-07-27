#include <core/ecs/entity.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_handle.hpp>

namespace args::core::ecs
{
	inline void entity::set_parent(id_type newParent)
	{
		if (m_parent != invalid_id)
			m_registry.getEntity(m_parent).m_children.erase(m_id);

		m_parent = newParent;

		if (m_parent != invalid_id)
			m_registry.getEntity(m_parent).m_children.insert(m_id);
	}

	A_NODISCARD inline entity& entity::operator[](index_type index)
	{
		return get_child(index);
	}

	A_NODISCARD inline entity& entity::get_child(index_type index)
	{
		return m_registry.getEntity(m_children[index]);
	}

	inline void entity::add_child(id_type childId)
	{
		if (!m_children.contains(childId))
			m_registry.getEntity(childId).set_parent(m_id);
	}

	inline void entity::remove_child(id_type childId)
	{
		if (m_children.contains(childId))
			m_registry.getEntity(childId).set_parent(invalid_id);
	}

	inline bool entity::has_component(id_type componentTypeId)
	{
		return m_components.contains(componentTypeId);
	}

	inline component_handle_base entity::get_component(id_type componentTypeId) const
	{
		return m_registry.getComponent(m_id, componentTypeId);
	}
}