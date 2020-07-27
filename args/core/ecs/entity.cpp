#include <core/ecs/entity.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_handle.hpp>

namespace args::core::ecs
{
	inline entity& entity::operator=(const entity& other)
	{
		m_id = other.m_id;
		m_registry = other.m_registry;
		return *this;
	}

	A_NODISCARD inline sparse_map<id_type, id_type>& entity::component_composition()
	{
		return m_registry->getEntityData(m_id).components;
	}

	A_NODISCARD id_type entity::get_id() const
	{
		valid();

		return m_id;
	}

	A_NODISCARD sparse_map<id_type, entity>::iterator entity::begin()
	{
		return m_registry->getEntityData(m_id).children.begin();
	}

	A_NODISCARD sparse_map<id_type, entity>::const_iterator entity::begin() const
	{
		return m_registry->getEntityData(m_id).children.begin();
	}

	A_NODISCARD sparse_map<id_type, entity>::iterator entity::end()
	{
		return m_registry->getEntityData(m_id).children.end();
	}

	A_NODISCARD sparse_map<id_type, entity>::const_iterator entity::end() const
	{
		return m_registry->getEntityData(m_id).children.end();
	}

	A_NODISCARD entity entity::get_parent() const
	{
		return m_registry->getEntity(m_registry->getEntityData(m_id).parent);
	}

	inline void entity::set_parent(id_type newParent)
	{
		entity_data& data = m_registry->getEntityData(m_id);

		if (data.parent != invalid_id)
			m_registry->getEntityData(data.parent).children.erase(m_id);

		data.parent = newParent;

		if (data.parent != invalid_id)
			m_registry->getEntityData(data.parent).children.insert(m_id, *this);
	}

	A_NODISCARD inline entity entity::operator[](index_type index) const
	{
		return get_child(index);
	}

	A_NODISCARD inline entity entity::get_child(index_type index) const
	{
		return m_registry->getEntityData(m_id).children.dense()[index];
	}

	A_NODISCARD size_type entity::child_count() const
	{
		return m_registry->getEntityData(m_id).children.size();
	}

	inline void entity::add_child(id_type childId)
	{
		entity_data& data = m_registry->getEntityData(m_id);
		if (!data.children.contains(childId))
			data.children[childId].set_parent(m_id);
	}

	inline void entity::remove_child(id_type childId)
	{
		entity_data& data = m_registry->getEntityData(m_id);
		if (!data.children.contains(childId))
			data.children[childId].set_parent(invalid_id);
	}

	A_NODISCARD inline bool entity::has_component(id_type componentTypeId) const
	{
		return m_registry->getEntityData(m_id).components.contains(componentTypeId);
	}

	A_NODISCARD inline component_handle_base entity::get_component(id_type componentTypeId) const
	{
		return m_registry->getComponent(m_id, componentTypeId);
	}

	inline component_handle_base entity::add_component(id_type componentTypeId)
	{
		return m_registry->createComponent(m_id, componentTypeId);
	}

	inline void entity::destroy()
	{
		m_registry->destroyEntity(m_id);
	}

	inline bool entity::valid() const
	{
		if (m_registry && m_id)
			if (m_registry->validateEntity(m_id))
				return true;

		m_id = invalid_id;

		return false;
	}
}