#pragma once
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include <core/containers/sparse_map.hpp>
#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/types.hpp>
#include <core/platform/platform.hpp>

#include <core/ecs/containers/component_pool.hpp>
#include <core/ecs/handles/entity.hpp>
#include <core/ecs/data/hierarchy.hpp>
#include <core/ecs/prototypes/entity_prototype.hpp>

namespace legion::core::ecs
{
#define world_entity_id 1

    class Registry
    {
    private:
        static std::unordered_map<id_type, std::unique_ptr<component_pool_base>> m_componentFamilies;
        static std::unordered_map<id_type, std::unordered_set<id_type>> m_entityComposition;
        static std::unordered_map<id_type, entity_hierarchy> m_entityHierarchy;

        template<typename component_type, typename... Args>
        L_NODISCARD static component_pool<component_type>* tryEmplaceFamily(Args&&... args);

    public:
        static entity world;

        template<typename component_type>
        L_NODISCARD static component_pool<component_type>* getFamily();

        static entity createEntity();
        static entity createEntity(entity parent);
        static entity createEntity(serialization::entity_prototype prototype);

        static void destroyEntity(entity target);
        static void destroyEntity(id_type target);

        static std::unordered_set<id_type>& entityComposition(entity target);
        static std::unordered_set<id_type>& entityComposition(id_type target);

        static entity_hierarchy& entityHierarchy(entity target);
        static entity_hierarchy& entityHierarchy(id_type target);
    };
}

#include <core/ecs/registry.inl>
