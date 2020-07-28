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
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->getEntityData(m_id).components;
	}

	A_NODISCARD id_type entity::get_id() const
	{
		if (valid())
			return m_id;
		return invalid_id;
	}

	A_NODISCARD sparse_map<id_type, entity>::iterator entity::begin()
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->getEntityData(m_id).children.begin();
	}

	A_NODISCARD sparse_map<id_type, entity>::const_iterator entity::begin() const
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->getEntityData(m_id).children.begin();
	}

	A_NODISCARD sparse_map<id_type, entity>::iterator entity::end()
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->getEntityData(m_id).children.end();
	}

	A_NODISCARD sparse_map<id_type, entity>::const_iterator entity::end() const
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->getEntityData(m_id).children.end();
	}

	A_NODISCARD entity entity::get_parent() const
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->getEntity(m_registry->getEntityData(m_id).parent);
	}

	inline void entity::set_parent(id_type newParent)
	{
		if (!m_registry)
			throw args_invalid_entity_error;
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
		if (!m_registry)
			throw args_invalid_entity_error;

		sparse_map<id_type, entity>& children = m_registry->getEntityData(m_id).children;
		if (index >= children.size())
			throw std::out_of_range("Child index out of range.");

		return children.dense()[index];
	}

	A_NODISCARD size_type entity::child_count() const
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->getEntityData(m_id).children.size();
	}

	inline void entity::add_child(id_type childId)
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		entity_data& data = m_registry->getEntityData(m_id);
		if (!data.children.contains(childId))
			data.children[childId].set_parent(m_id);
	}

	inline void entity::remove_child(id_type childId)
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		entity_data& data = m_registry->getEntityData(m_id);
		if (!data.children.contains(childId))
			data.children[childId].set_parent(invalid_id);
	}

	A_NODISCARD inline bool entity::has_component(id_type componentTypeId) const
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->getEntityData(m_id).components.contains(componentTypeId);
	}

	A_NODISCARD inline component_handle_base entity::get_component(id_type componentTypeId) const
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->getComponent(m_id, componentTypeId);
	}

	inline component_handle_base entity::add_component(id_type componentTypeId)
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		return m_registry->createComponent(m_id, componentTypeId);
	}

	void entity::remove_component(id_type componentTypeId)
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		m_registry->destroyComponent(m_id, componentTypeId);
	}

	inline void entity::destroy(bool recurse)
	{
		if (!m_registry)
			throw args_invalid_entity_error;
		m_registry->destroyEntity(m_id);
	}

	inline bool entity::valid() const
	{
		if (m_registry && m_id)
			if (m_registry->validateEntity(m_id))
				return true;

		return false;
	}
}