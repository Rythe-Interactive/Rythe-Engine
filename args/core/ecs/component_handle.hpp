#pragma once
#include <atomic>
#include <core/common/exception.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_container.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/platform/platform.hpp>

/**
 * @file component_handle.hpp
 */

namespace args::core::ecs
{
    template<typename component_type>
    class component_handle;

    /**@class component_handle_base
     * @brief Base class of args::core::ecs::component_handle.
     * @ref args::core::ecs::component_handle.
     */
    class component_handle_base
    {
    public:
        // Entity that owns this component.
        const entity_handle entity;

    protected:
        EcsRegistry* m_registry;
        id_type m_ownerId;

    public:
        component_handle_base() : m_registry(nullptr), m_ownerId(invalid_id) {};
        component_handle_base(const component_handle_base& other) : m_registry(other.m_registry), m_ownerId(other.m_ownerId) {};
        component_handle_base(component_handle_base&& other) : m_registry(other.m_registry), m_ownerId(other.m_ownerId) {};
        component_handle_base(id_type entityId, EcsRegistry* registry) : entity(registry->getEntity(entityId)), m_registry(registry), m_ownerId(entityId) {}

        component_handle_base& operator=(const component_handle_base& other) { m_registry = other.m_registry; m_ownerId = other.m_ownerId; return *this; }
        component_handle_base& operator=(component_handle_base&& other) { m_registry = other.m_registry; m_ownerId = other.m_ownerId; return *this; }

        /**@brief Checks if handle still points to a valid component.
         */
        A_NODISCARD virtual bool valid() ARGS_IMPURE_RETURN(m_ownerId);

        /**@brief Checks if handle still points to a valid component.
         */
        operator bool() { return valid(); }

        template<typename component_type>
        component_handle<component_type> cast();
    };

    /**@class component_handle
     * @brief Handle to components that allow safe component loading and storing.
     * @tparam component_type Type of targeted component.
     */
    template<typename component_type>
    class component_handle : public component_handle_base
    {
        friend class std::hash<component_handle<component_type>>;
    public:
        component_handle() : component_handle_base() {}
        component_handle(const component_handle& other) : component_handle_base(other.m_ownerId, other.m_registry) {};
        component_handle(component_handle&& other) : component_handle_base(other.m_ownerId, other.m_registry) {};

        /**@brief Creates component handle for the given entity.
         */
        component_handle(id_type entityId, EcsRegistry* registry) : component_handle_base(entityId, registry) {}

        component_handle& operator=(const component_handle& other) { m_registry = other.m_registry; m_ownerId = other.m_ownerId; return *this; }
        component_handle& operator=(component_handle&& other) { m_registry = other.m_registry; m_ownerId = other.m_ownerId; return *this; }

        bool operator==(const component_handle<component_type>& other) const { return m_registry == other.m_registry && m_ownerId == other.m_ownerId; }

        /**@brief Thread-safe read of component.
         * @returns component_type Current value of component.
         */
        A_NODISCARD component_type read()
        {
            if (!m_ownerId || !m_registry)
                return component_type();

            component_container<component_type>* family = m_registry->getFamily<component_type>();

            async::readonly_guard rguard(family->get_lock());

            if (!family->has_component(m_ownerId))
                return component_type();

            return family->get_component(m_ownerId);
        }

        /**@brief Thread-safe write of component.
         * @param value Value you wish to write.
         * @returns component_type Current value of component.
         */
        component_type write(component_type&& value)
        {
            if (!m_ownerId || !m_registry)
                return component_type();

            component_container<component_type>* family = m_registry->getFamily<component_type>();

            async::readonly_guard rguard(family->get_lock());

            if (!family->has_component(m_ownerId))
                return component_type();

            family->get_component(m_ownerId) = value;
            return value;
        }

        /**@brief Thread-safe write of component.
         * @param value Value you wish to write.
         * @returns component_type Current value of component.
         */
        component_type write(const component_type& value)
        {
            if (!m_ownerId || !m_registry)
                return component_type();

            component_container<component_type>* family = m_registry->getFamily<component_type>();

            async::readonly_guard rguard(family->get_lock());

            if (!family->has_component(m_ownerId))
                return component_type();

            family->get_component(m_ownerId) = value;
            return value;
        }

        /**@brief Thread-safe read modify write with a custom modification on component.
         * @param value Value you wish to add.
         * @returns component_type Current value of component.
         */
        component_type read_modify_write(component_type&& value, component_type(*modifier)(const component_type&, component_type&&))
        {
            if (!m_ownerId || !m_registry)
                return component_type();

            component_container<component_type>* family = m_registry->getFamily<component_type>();

            async::readonly_guard rguard(family->get_lock());

            if (!family->has_component(m_ownerId))
                return component_type();

            component_type& comp = family->get_component(m_ownerId);
            comp = modifier(comp, std::forward<component_type>(value));
            return comp;
        }

        /**@brief Thread-safe read modify write with add modification on component.
         * @param value Value you wish to add.
         * @returns component_type Current value of component.
         */
        component_type fetch_add(component_type&& value)
        {
            if (!m_ownerId || !m_registry)
                return component_type();

            component_container<component_type>* family = m_registry->getFamily<component_type>();

            async::readonly_guard rguard(family->get_lock());

            if (!family->has_component(m_ownerId))
                return component_type();

            component_type& comp = family->get_component(m_ownerId);
            comp = comp + value;
            return comp;
        }

        /**@brief Thread-safe read modify write with add modification on component.
         * @param value Value you wish to add.
         * @returns component_type Current value of component.
         */
        component_type fetch_add(const component_type& value)
        {
            if (!m_ownerId || !m_registry)
                return component_type();

            component_container<component_type>* family = m_registry->getFamily<component_type>();

            async::readonly_guard rguard(family->get_lock());

            if (!family->has_component(m_ownerId))
                return component_type();

            component_type& comp = family->get_component(m_ownerId);
            comp = comp + value;
            return comp;
        }

        /**@brief Thread-safe read modify write with multiply modification on component.
         * @param value Value you wish to multiply by.
         * @returns component_type Current value of component.
         */
        component_type fetch_multiply(component_type&& value)
        {
            if (!m_ownerId || !m_registry)
                return component_type();

            component_container<component_type>* family = m_registry->getFamily<component_type>();

            async::readonly_guard rguard(family->get_lock());

            if (!family->has_component(m_ownerId))
                return component_type();

            component_type& comp = family->get_component(m_ownerId);
            comp = comp * value;
            return comp;
        }

        /**@brief Thread-safe read modify write with multiply modification on component.
         * @param value Value you wish to multiply by.
         * @returns component_type Current value of component.
         */
        component_type fetch_multiply(const component_type& value)
        {
            if (!m_ownerId || !m_registry)
                return component_type();

            component_container<component_type>* family = m_registry->getFamily<component_type>();

            async::readonly_guard rguard(family->get_lock());

            if (!family->has_component(m_ownerId))
                return component_type();

            component_type& comp = family->get_component(m_ownerId);
            comp = comp * value;
            return comp;
        }

        /**@brief Locks component family and destroys component.
         * @ref args::core::ecs::component_container::destroy_component
         */
        void destroy()
        {
            if (!m_ownerId || !m_registry)
                return;

            m_registry->getFamily<component_type>()->destroy_component(m_ownerId);
        }

        /**@brief Checks if handle still points to a valid component.
         */
        virtual bool valid() override
        {
            return m_ownerId && m_registry && m_registry->getFamily<component_type>()->has_component(m_ownerId);
        }
    };


    template<typename component_type>
    inline component_handle<component_type> component_handle_base::cast()
    {
        return component_handle<component_type>(m_ownerId, m_registry);
    }
}

namespace std
{
    template<typename component_type> struct hash<args::core::ecs::component_handle<component_type>>
    {
        std::size_t operator()(args::core::ecs::component_handle<component_type> const& handle) const noexcept
        {
            std::size_t h1 = std::hash<intptr_t>{}(handle.m_registry);
            std::size_t h2 = std::hash<args::core::id_type>{}(handle.m_ownerId);
            return h1 ^ (h2 << 1);
        }
    };
}
