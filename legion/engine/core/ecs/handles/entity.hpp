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

        struct entity
        {
        public:
            id_type id = invalid_id;

            operator const id_type& () const;
            operator id_type& ();

            void set_parent(id_type parent) const;
            void set_parent(entity parent) const;

            void add_child(id_type child) const;
            void add_child(entity child) const;

            void remove_child(id_type child) const;
            void remove_child(entity child) const;

            template<typename component_type>
            bool add_component();

            template<typename component_type>
            bool add_component(const serialization::component_prototype<component_type>& prot);

            template<typename component_type>
            bool add_component(serialization::component_prototype<component_type>&& prot);

            template<typename component_type>
            bool has_component();
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
            return std::hash<legion::core::id_type>{}(handle.id);
        }
    };
}
#endif
