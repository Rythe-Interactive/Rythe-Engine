#pragma once
#include <core/types/types.hpp>
#include <core/containers/sparse_set.hpp>
#include <core/containers/sparse_map.hpp>

/// <summary>
/// TODO: entity::isvalid()
/// </summary>

namespace args::core::ecs
{
	class ARGS_API EcsRegistry;

	class component_handle_base;

	template<typename component_type>
	class component_handle;

	class ARGS_API entity
	{
		friend class EcsRegistry;
	private:
		mutable id_type m_id;
		EcsRegistry* m_registry;

	public:
		entity(id_type id, EcsRegistry* registry) : m_id(id) { m_registry = registry; }
		entity() : m_id(invalid_id) { m_registry = nullptr; }
		entity(const entity& other) : m_id(other.m_id) { m_registry = other.m_registry; }
		entity& operator=(const entity& other);

		A_NODISCARD sparse_map<id_type, id_type>& component_composition();

		operator id_type() const { return get_id(); }
		A_NODISCARD id_type get_id() const;

		A_NODISCARD sparse_map<id_type, entity>::iterator begin();
		A_NODISCARD sparse_map<id_type, entity>::const_iterator begin() const;

		A_NODISCARD sparse_map<id_type, entity>::iterator end();
		A_NODISCARD sparse_map<id_type, entity>::const_iterator end() const;

		A_NODISCARD entity get_parent() const;
		void set_parent(id_type newParent);

		A_NODISCARD entity operator[] (index_type index) const;
		A_NODISCARD entity get_child(index_type index) const;
		A_NODISCARD size_type child_count() const;

		void add_child(id_type childId);
		void remove_child(id_type childId);

		template<typename component_type>
		A_NODISCARD bool has_component() const { return has_component(typeHash<component_type>()); }

		A_NODISCARD bool has_component(id_type componentTypeId) const;

		A_NODISCARD component_handle_base get_component(id_type componentTypeId) const;

		template<typename component_type>
		A_NODISCARD component_handle<component_type> get_component() const
		{
			return force_value_cast<component_handle<component_type>>(get_component(typeHash<component_type>()));
		}

		component_handle_base add_component(id_type componentTypeId);

		template<typename component_type>
		component_handle<component_type> add_component()
		{
			return force_value_cast<component_handle<component_type>>(add_component(typeHash<component_type>()));
		}

		void destroy();

		bool valid() const;
	};
}