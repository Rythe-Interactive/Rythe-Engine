#pragma once
#include <unordered_map>
#include <core/types/types.hpp>
#include <core/ecs/component_container.hpp>
#include <core/common/common.hpp>

namespace args::core::ecs
{
	class EcsRegistry
	{
	private:
		std::unordered_map<id_type, component_container_base*> families;

	public:

		template<typename component_type>
		component_container<component_type>* getFamily()
		{
			id_type componentTypeId = typeHash<component_type>();

			assert_msg("Component type unknown.", families.count(componentTypeId));

			return static_cast<component_container<component_type>*>(families[componentTypeId]);
		}
	};
}