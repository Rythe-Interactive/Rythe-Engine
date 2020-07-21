#pragma once
#include <core/types/types.hpp>
#include <core/containers/sparse_set.hpp>

namespace args::core::ecs
{
	class EcsRegistry;

	class component_handle_base;

	template<typename component_type>
	class component_handle;

	class entity
	{
		friend class EcsRegistry;
	private:
		id_type m_id;
		EcsRegistry& m_registry;

		id_type m_parent;
		sparse_set<id_type> m_children;
		sparse_set<id_type> m_components;

	public:
		entity(id_type id, EcsRegistry& registry) : m_id(id), m_registry(registry), m_parent(invalid_id), m_children() {}

		operator id_type() const { return m_id; }

		A_NODISCARD sparse_set<id_type>::iterator begin() { return m_children.begin(); }
		A_NODISCARD sparse_set<id_type>::const_iterator begin() const { return m_children.begin(); }

		A_NODISCARD sparse_set<id_type>::iterator end() { return m_children.end(); }
		A_NODISCARD sparse_set<id_type>::const_iterator end() const { return m_children.end(); }

		A_NODISCARD id_type get_parent() { return m_parent; }
		void set_parent(id_type newParent);

		A_NODISCARD entity& operator[] (index_type index);
		A_NODISCARD entity& get_child(index_type index);
		A_NODISCARD size_type child_count() { return m_children.size(); }

		void add_child(id_type childId);
		void remove_child(id_type childId);

		template<typename component_type>
		bool has_component() { return has_component(typeHash<component_type>()); }

		bool has_component(id_type componentTypeId);

		template<typename component_type>
		component_handle<component_type> get_component() const
		{
			return static_cast<component_handle<component_type>>(getComponent(typeHash<component_type>()));
		}

		component_handle_base get_component(id_type componentTypeId) const;
	};
}