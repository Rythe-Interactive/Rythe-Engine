#pragma once
#include <core/async/readonly_rw_spinlock.hpp>
#include <core/async/transferable_atomic.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/atomic_sparse_map.hpp>
#include <core/types/types.hpp>
#include <core/events/eventbus.hpp>
#include <core/events/events.hpp>
#include <core/ecs/component_meta.hpp>

/**
 * @file component_container.hpp
 */

namespace args::core::ecs
{
    class ARGS_API EcsRegistry;

    /**@class component_container_base
     * @brief Base class of args::core::ecs::component_container
     */
    class component_container_base
    {
    public:
        virtual bool has_component(id_type entityId) ARGS_PURE;
        virtual void create_component(id_type entityId) ARGS_PURE;
        virtual void create_component(id_type entityId, void* value) ARGS_PURE;
        virtual void destroy_component(id_type entityId) ARGS_PURE;
        virtual ~component_container_base() = default;
    };

    /**@class component_container
     * @brief Thread-safe container to store a component family in.
     * @tparam component_type Type of component.
     */
    template<typename component_type>
    class component_container : public component_container_base
    {
    private:
        atomic_sparse_map<id_type, component_type> components;

        events::EventBus* m_eventBus;
        EcsRegistry* m_registry;

    public:
        component_container() = default;
        component_container(EcsRegistry* registry, events::EventBus* eventBus) : m_registry(registry), m_eventBus(eventBus) {}

        ~component_container()
        {
            auto entities = components.keys();
            auto count = components.size();

            for (int i = 0; i < count; i++)
                destroy_component(entities[i]);
        }

        /**@brief Checks whether entity has the component.
         * @note Thread will be halted if there are any writes until they are finished.
         * @note Will trigger read on this container.
         * @param entityId ID of the entity you wish to check for.
         * @ref args::core::async::readonly_rw_spinlock
         */
        virtual bool has_component(id_type entityId) override
        {
            return components.contains(entityId);
        }

        /**@brief Fetches std::atomic wrapped component.
         * @note Thread will be halted if there are any writes until they are finished.
         * @note Will trigger read on this container.
         * @param entityId ID of entity you want to get the component from.
         * @returns std::atomic<component_type>* Pointer to std::atomic wrapped component.
         * @ref args::core::async::readonly_rw_spinlock
         */
        async::transferable_atomic<component_type>* get_component(id_type entityId)
        {
            if (components.contains(entityId))
                return &components.get(entityId);
            return nullptr;
        }

        /**@brief Creates new std::atomic wrapped component.
         * @note Thread will be halted if there are any reads or writes until they are finished.
         * @note Will trigger write on this container.
         * @param entityId ID of entity you wish to add the component to.
         * @ref args::core::async::readonly_rw_spinlock
         */
        virtual void create_component(id_type entityId) override
        {
            component_type comp;
            if constexpr (detail::has_init<component_type, void(component_type&)>::value)
                component_type::init(comp);
            components.emplace(entityId, comp);
            m_eventBus->raiseEvent<events::component_creation<component_type>>(entity_handle(entityId, m_registry));
        }

        virtual void create_component(id_type entityId, void* value) override
        {
            components.emplace(entityId, *reinterpret_cast<component_type*>(value));
            m_eventBus->raiseEvent<events::component_creation<component_type>>(entity_handle(entityId, m_registry));
        }

        /**@brief Destroys component atomically.
         * @note Thread will be halted if there are any reads or writes until they are finished.
         * @note Will trigger write on this container.
         * @param entityId ID of entity you wish to remove the component from.
         * @ref args::core::async::readonly_rw_spinlock
         */
        virtual void destroy_component(id_type entityId) override
        {            
            m_eventBus->raiseEvent<events::component_destruction<component_type>>(entity_handle(entityId, m_registry));
            component_type comp = components[entityId]->load(std::memory_order_acquire);
            if constexpr (detail::has_destroy<component_type, void(component_type&)>::value)
                component_type::destroy(comp);
            components.erase(entityId);
        }
    };
}
