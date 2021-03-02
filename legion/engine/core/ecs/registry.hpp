#pragma once
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include <core/containers/sparse_map.hpp>
#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/types.hpp>
#include <core/platform/platform.hpp>
#include <core/common/hash.hpp>

#include <core/ecs/containers/component_pool.hpp>
#include <core/ecs/handles/entity.hpp>
#include <core/ecs/data/hierarchy.hpp>
#include <core/ecs/prototypes/entity_prototype.hpp>

namespace legion::core::ecs
{
    class Registry
    {
        template<typename component_type>
        friend struct component_pool;

    private:
        static std::unordered_map<id_type, std::unique_ptr<component_pool_base>> m_componentFamilies;
        static std::unordered_map<entity, std::unordered_set<id_type>> m_entityComposition;
        static std::unordered_map<entity, entity_hierarchy> m_entityHierarchy;
        static std::queue<entity> m_recyclableEntities;

        template<typename component_type, typename... Args>
        L_NODISCARD static component_pool<component_type>* tryEmplaceFamily(Args&&... args);

    public:
        static constexpr entity world = { world_entity_id };

        template<typename component_type>
        L_NODISCARD static component_pool<component_type>* getFamily();

        L_NODISCARD static component_pool_base* getFamily(id_type typeId);

        static entity createEntity(entity parent = world);
        static entity createEntity(entity parent, const serialization::entity_prototype& prototype);
        static entity createEntity(const serialization::entity_prototype& prototype);

        static void destroyEntity(entity target, bool recurse = true);
        static void destroyEntity(id_type target, bool recurse = true);

        L_NODISCARD static std::unordered_map<entity, std::unordered_set<id_type>>& entityCompositions();

        L_NODISCARD static std::unordered_set<id_type>& entityComposition(entity target);
        L_NODISCARD static std::unordered_set<id_type>& entityComposition(id_type target);

        L_NODISCARD static entity_hierarchy& entityHierarchy(entity target);
        L_NODISCARD static entity_hierarchy& entityHierarchy(id_type target);

        template<typename component_type>
        static component_type& createComponent(entity target);
        template<typename component_type>
        static component_type& createComponent(entity target, const serialization::component_prototype<component_type>& prototype);

        static void* createComponent(id_type typeId, entity target);
        static void* createComponent(id_type typeId, entity target, const serialization::component_prototype_base& prototype);

        template<typename component_type>
        static void destroyComponent(entity target);

        static void destroyComponent(id_type typeId, entity target);

        template<typename component_type>
        static component_type& getComponent(entity target);

        static void* getComponent(id_type typeId, entity target);
    };
}

#include <core/ecs/registry.inl>
#include <core/ecs/containers/component_pool.inl>
#include <core/ecs/filters/filterregistry.inl>
