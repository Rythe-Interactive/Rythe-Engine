#pragma once
#include <core/types/types.hpp>

namespace legion::core
{
    namespace ecs
    {
        struct component_base;

        template<typename component_type>
        struct component;
    }

    namespace serialization
    {
        template<typename T>
        struct prototype;

        using component_prototype_base = prototype<ecs::component_base>;

        template<typename component_type>
        using component_prototype = prototype<ecs::component<component_type>>;
    }

    namespace ecs
    {
        static constexpr id_type world_entity_id = 1;

        /**@struct entity
         * @brief Handle to an entity.
         */
        struct entity
        {
        public:
            id_type id = invalid_id;

            bool operator ==(std::nullptr_t) const;
            bool operator !=(std::nullptr_t) const;

            operator const id_type& () const noexcept;
            operator id_type& () noexcept;

            void set_parent(id_type parent);
            void set_parent(entity parent);

            entity get_parent() const;

            void add_child(id_type child);
            void add_child(entity child);

            void remove_child(id_type child);
            void remove_child(entity child);

            void destroy(bool recurse = true);

            template<typename component_type>
            component<component_type> add_component();

            template<typename component_type>
            component<component_type> add_component(const serialization::component_prototype<component_type>& prot);

            template<typename component_type>
            component<component_type> add_component(serialization::component_prototype<component_type>&& prot);

            template<typename component_type>
            bool has_component() const;

            template<typename component_type>
            component<component_type> get_component();

            template<typename component_type>
            const component<component_type> get_component() const;

            template<typename component_type>
            void remove_component();
        };
    }
}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<> struct hash<legion::core::ecs::entity>
    {
        std::size_t operator()(legion::core::ecs::entity const& handle) const noexcept
        {
            return handle.id;
        }
    };
}
#endif
