#pragma once
#include <core/types/types.hpp>

namespace args::core::ecs
{
	class EcsRegistry;

	class entity
	{
	private:
		id_type id;
		EcsRegistry* registry;

	public:
		entity(id_type id, EcsRegistry* registry) : id(id), registry(registry) {}

		operator id_type() const { return id; }
	};
}