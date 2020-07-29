#pragma once
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/sparse_map.hpp>

/**
 * @file entityquery.hpp
 */

namespace args::core::ecs
{
	class ARGS_API QueryRegistry;
	class ARGS_API EcsRegistry;
	class ARGS_API entity_handle;

	/**@class EntityQuery
	 * @brief .
	 */
	class ARGS_API EntityQuery
	{
	private:
		QueryRegistry& m_registry;
		EcsRegistry& m_ecsRegistry;
		id_type m_id;

	public:
		EntityQuery(id_type id, QueryRegistry& registry, EcsRegistry& ecsRegistry) : m_registry(registry), m_ecsRegistry(ecsRegistry), m_id(id) {}
		~EntityQuery();

		sparse_map<id_type, entity_handle>::const_iterator begin() const;
		sparse_map<id_type, entity_handle>::const_iterator end() const;

		/**@brief Get query id.
		 */
		A_NODISCARD id_type id() { return m_id; }

		/**@brief .
		 * @tparam component_type
		 */
		template<typename component_type>
		void addComponentType() { addComponentType(typeHash<component_type>()); }

		/**@brief .
		 * @param componentTypeId
		 */
		void addComponentType(id_type componentTypeId);

		/**@brief .
		 * @tparam component_type
		 */
		template<typename component_type>
		void removeComponentType() { removeComponentType(typeHash<component_type>()); }

		/**@brief .
		 * @param componentTypeId
		 */
		void removeComponentType(id_type componentTypeId);

		/**@brief .
		 */
		entity_handle operator[](size_type index);

		/**@brief .
		 */
		size_type size();
	};
}