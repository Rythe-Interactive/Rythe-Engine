#pragma once
#include <atomic>
#include <core/common/exception.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_pool.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/platform/platform.hpp>

#include <Optick/optick.h>

/**
 * @file component_handle.hpp
 */
namespace legion::core
{
    class Engine;
}

namespace legion::core::ecs
{
    template<typename component_type>
    class component_handle;

    /**@class component_handle_base
     * @brief Base class of legion::core::ecs::component_handle.
     * @ref legion::core::ecs::component_handle.
     */
    class component_handle_base
    {
        friend class legion::core::Engine;
    public:
        // Entity that owns this component.
        entity_handle entity;

    protected:
        id_type m_typeId;
        static EcsRegistry* m_registry;
        static events::EventBus* m_eventBus;
        
    public:
        component_handle_base() : entity(), m_typeId(invalid_id) {};
        component_handle_base(const component_handle_base& other) : entity(other.entity), m_typeId(other.m_typeId) {};
        component_handle_base(component_handle_base&& other) : entity(other.entity), m_typeId(other.m_typeId) {};
        component_handle_base(id_type entityId, id_type typeId) : entity(entityId), m_typeId(typeId) {}

        component_handle_base& operator=(const component_handle_base& other) { entity = other.entity; m_typeId = other.m_typeId; return *this; }
        component_handle_base& operator=(component_handle_base&& other) { entity = other.entity; m_typeId = other.m_typeId; return *this; }

        template<typename component_type>
        component_handle<component_type> cast();
        template<typename component_type>
        const component_handle<component_type> cast() const;

        void serialize(cereal::JSONOutputArchive& oarchive);
        void serialize(cereal::BinaryOutputArchive& oarchive);
        void serialize(cereal::JSONInputArchive& oarchive);
        void serialize(cereal::BinaryInputArchive& oarchive);

        /**@brief Checks if handle still points to a valid component.
         */
        L_NODISCARD virtual bool valid() const LEGION_IMPURE_RETURN(entity.valid());

        /**@brief Checks if handle still points to a valid component.
         */
        operator bool() const { return valid(); }
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
        component_handle(const component_handle& other) : component_handle_base(other) {};
        component_handle(component_handle&& other) : component_handle_base(other) {};

        /**@brief Creates component handle for the given entity.
         */
        component_handle(id_type entityId) : component_handle_base(entityId,typeHash<component_type>()) {}

        component_handle& operator=(const component_handle& other) { entity = other.entity; m_typeId = other.m_typeId; return *this; }
        component_handle& operator=(component_handle&& other) { entity = other.entity; m_typeId = other.m_typeId; return *this; }
                                                                                       
        bool operator==(const component_handle<component_type>& other) const { return entity == other.entity && m_typeId == other.m_typeId; }

        /**@brief Thread-safe read of component.
         * @param order Memory order at which to load the component.
         * @returns component_type Current value of component.
         */
        L_NODISCARD component_type read() const
        {
            OPTICK_EVENT();
            component_pool<component_type>* family = m_registry->getFamily<component_type>();

            async::readonly_guard rguard(family->get_lock());

            return family->get_component(entity);
        }

        /**@brief Thread-safe write of component.
         * @param value Value you wish to write.
         * @returns component_type Current value of component.
         */
        component_type write(component_type&& value)
        {
            OPTICK_EVENT();

            component_pool<component_type>* family = m_registry->getFamily<component_type>();

            component_type old;
            {
                async::readonly_guard rguard(family->get_lock());

#ifdef LGN_SAFE_MODE
                if (!family->has_component(entity))
                    return component_type();
#endif

                component_type& ref = family->get_component(entity);
                old = ref;
                ref = value;
            }
            m_eventBus->raiseEvent<events::component_modification<component_type>>(entity, std::move(old), std::cref(value));
            return value;
        }

        /**@brief Thread-safe write of component.
         * @param value Value you wish to write.
         * @returns component_type Current value of component.
         */
        component_type write(const component_type& value)
        {
            OPTICK_EVENT();

            component_pool<component_type>* family = m_registry->getFamily<component_type>();

            component_type old;
            {
                async::readonly_guard rguard(family->get_lock());

#ifdef LGN_SAFE_MODE
                if (!family->has_component(entity))
                    return component_type();
#endif

                component_type& ref = family->get_component(entity);
                old = ref;
                ref = value;
            }
            m_eventBus->raiseEvent<events::component_modification<component_type>>(entity, std::move(old), value);
            return value;
        }

        /**@brief Thread-safe read modify write with a custom modification on component.
         * @param value Value you wish to add.
         * @returns component_type Current value of component.
         */
        template<typename Func>
        component_type read_modify_write(Func&& modifier)
        {
            OPTICK_EVENT();

            component_pool<component_type>* family = m_registry->getFamily<component_type>();

            component_type ret;
            component_type old;
            {
                async::readonly_guard rguard(family->get_lock());

#ifdef LGN_SAFE_MODE
                if (!family->has_component(entity))
                    return component_type();
#endif

                component_type& comp = family->get_component(entity);
                old = comp;
                modifier(comp);
                ret = comp;
            }
            m_eventBus->raiseEvent<events::component_modification<component_type>>(entity, std::move(old), ret);
            return ret;
        }

        template<typename Func>
        component_type read_modify_write(const Func& modifier)
        {
            OPTICK_EVENT();

            component_pool<component_type>* family = m_registry->getFamily<component_type>();

            component_type ret;
            component_type old;
            {
                async::readonly_guard rguard(family->get_lock());

#ifdef LGN_SAFE_MODE
                if (!family->has_component(entity))
                    return component_type();
#endif

                component_type& comp = family->get_component(entity);
                old = comp;
                modifier(comp);
                ret = comp;
            }
            m_eventBus->raiseEvent<events::component_modification<component_type>>(entity, std::move(old), ret);
            return ret;
        }

        /**@brief Thread-safe read modify write with add modification on component.
         * @param value Value you wish to add.
         * @returns component_type Current value of component.
         */
        component_type fetch_add(component_type&& value)
        {
            OPTICK_EVENT();

            component_pool<component_type>* family = m_registry->getFamily<component_type>();

            component_type ret;
            component_type old;
            {
                async::readonly_guard rguard(family->get_lock());

#ifdef LGN_SAFE_MODE
                if (!family->has_component(entity))
                    return component_type();
#endif

                component_type& comp = family->get_component(entity);
                old = comp;
                comp = comp + value;
                ret = comp;
            }
            m_eventBus->raiseEvent<events::component_modification<component_type>>(entity, std::move(old), ret);
            return ret;
        }

        /**@brief Thread-safe read modify write with add modification on component.
         * @param value Value you wish to add.
         * @returns component_type Current value of component.
         */
        component_type fetch_add(const component_type& value)
        {
            OPTICK_EVENT();

            component_pool<component_type>* family = m_registry->getFamily<component_type>();

            component_type ret;
            component_type old;
            {
                async::readonly_guard rguard(family->get_lock());

#ifdef LGN_SAFE_MODE
                if (!family->has_component(entity))
                    return component_type();
#endif

                component_type& comp = family->get_component(entity);
                old = comp;
                comp = comp + value;
                ret = comp;
            }
            m_eventBus->raiseEvent<events::component_modification<component_type>>(entity, std::move(old), ret);
            return ret;
        }

        /**@brief Thread-safe read modify write with multiply modification on component.
         * @param value Value you wish to multiply by.
         * @returns component_type Current value of component.
         */
        component_type fetch_multiply(component_type&& value)
        {
            OPTICK_EVENT();

            component_pool<component_type>* family = m_registry->getFamily<component_type>();

            component_type ret;
            component_type old;
            {
                async::readonly_guard rguard(family->get_lock());

#ifdef LGN_SAFE_MODE
                if (!family->has_component(entity))
                    return component_type();
#endif

                component_type& comp = family->get_component(entity);
                old = comp;
                comp = comp * value;
                ret = comp;
            }
            m_eventBus->raiseEvent<events::component_modification<component_type>>(entity, std::move(old), ret);
            return ret;
        }

        /**@brief Thread-safe read modify write with multiply modification on component.
         * @param value Value you wish to multiply by.
         * @returns component_type Current value of component.
         */
        component_type fetch_multiply(const component_type& value)
        {
            OPTICK_EVENT();

            component_pool<component_type>* family = m_registry->getFamily<component_type>();

            component_type ret;
            component_type old;
            {
                async::readonly_guard rguard(family->get_lock());

#ifdef LGN_SAFE_MODE
                if (!family->has_component(entity))
                    return component_type();
#endif

                component_type& comp = family->get_component(entity);
                old = comp;
                comp = comp * value;
                ret = comp;
            }
            m_eventBus->raiseEvent<events::component_modification<component_type>>(entity, std::move(old), ret);
            return ret;
        }

        /**@brief Locks component family and destroys component.
         * @ref legion::core::ecs::component_pool::destroy_component
         */
        void destroy()
        {
            OPTICK_EVENT();
            m_registry->destroyComponent(entity, typeHash<component_type>());
        }

        /**@brief Checks if handle still points to a valid component.
         */
        virtual bool valid() const override
        {
            OPTICK_EVENT();
            return m_registry->getFamily<component_type>()->has_component(entity);
        }
    };


    template<typename component_type>
    inline component_handle<component_type> component_handle_base::cast()
    {
        OPTICK_EVENT();
        if (typeHash<component_type>() == m_typeId)
        {
            return component_handle<component_type>(entity);
        }
        else
        {
            return component_handle<component_type>();
        }
    }

    template<typename component_type>
    inline const component_handle<component_type> component_handle_base::cast() const
    {
        OPTICK_EVENT();
        if (typeHash<component_type>() == m_typeId)
        {
            return component_handle<component_type>(entity);
        }
        else
        {
            return component_handle<component_type>();
        }
    }
}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<typename component_type> struct hash<legion::core::ecs::component_handle<component_type>>
    {
        std::size_t operator()(legion::core::ecs::component_handle<component_type> const& handle) const noexcept
        {
            OPTICK_EVENT();
            std::size_t hash;
            std::size_t h1 = std::hash<intptr_t>{}(reinterpret_cast<intptr_t>(handle.m_registry));
            std::size_t h2 = std::hash<legion::core::id_type>{}(handle.entity);
            std::size_t h3 = legion::core::typeHash<component_type>();
            hash = h1 ^ (h2 << 1);
            return hash ^ (h3 << 1);
        }
    };
}
#endif
