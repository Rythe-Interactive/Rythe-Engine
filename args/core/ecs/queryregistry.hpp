#pragma once
#include <vector>
#include <core/containers/containers.hpp>
#include <core/types/primitives.hpp>

namespace args::core::ecs
{
	class ARGS_API QueryRegistry
	{
	private:
		std::vector<sparse_set<id_type>> m_entityLists;
		std::vector<sparse_map<id_type, id_type>> m_componentTypes;

	public:
		void addComponentType(id_type queryId, id_type componentTypeId)
		{
			m_componentTypes[queryId].insert(componentTypeId, componentTypeId);
		}

		void removeComponentType(id_type queryId, id_type componentTypeId)
		{
			m_componentTypes[queryId].erase(componentTypeId);
		}
	};

}