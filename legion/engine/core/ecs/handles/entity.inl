#include <core/ecs/handles/entity.hpp>
#include <core/ecs/registry.hpp>
#pragma once

#if !defined(DOXY_EXCLUDE)
namespace std
{
    inline size_t hash<legion::core::ecs::entity>::operator()(legion::core::ecs::entity const& handle) const noexcept
    {
        return handle->id;
    }
}
#endif

namespace legion::core::ecs
{
    template<>
    inline bool entity::operator==<std::nullptr_t>(std::nullptr_t) const
    {
        return !(data && data->alive);
    }

    template<>
    inline bool entity::operator!=<std::nullptr_t>(std::nullptr_t) const
    {
        return data && data->alive;
    }

    template<>
    inline bool entity::operator==<id_type>(id_type id) const
    {
        return data && data->alive ? data->id == id : id == invalid_id;
    }

    template<>
    inline bool entity::operator!=<id_type>(id_type id) const
    {
        return data && data->alive && (data->id != id || id == invalid_id);
    }

    template<>
    inline bool entity::operator==<entity>(entity other) const
    {
        return data && data->alive && other.data && data->id == other->id;
    }

    template<>
    inline bool entity::operator!=<entity>(entity other) const
    {
        return data && data->alive && other.data && data->id != other->id;
    }

    template<typename T>
    inline bool entity::operator==(T val) const
    {
        return data && data->alive ? data->id == val : val == invalid_id;
    }

    template<typename T>
    inline bool entity::operator!=(T val) const
    {
        return data && data->alive && (data->id != val || val == invalid_id);
    }

    template<typename component_type>
    inline component<component_type> entity::add_component()
    {
        Registry::createComponent<component_type>(*this);
        return { {}, entity{data} };
    }

    template<typename component_type>
    inline component<component_type> entity::add_component(const serialization::prototype<component<component_type>>& prot)
    {
        Registry::createComponent<component_type>(*this, prot);
        return { {}, entity{data} };
    }

    template<typename component_type>
    inline component<component_type> entity::add_component(serialization::prototype<component<component_type>>&& prot)
    {
        Registry::createComponent<component_type>(*this, std::move(prot));
        return { {}, entity{data} };
    }

    template<typename component_type>
    inline bool entity::has_component() const
    {
        return Registry::hasComponent<component_type>(*this);
    }

    template<typename component_type>
    inline component<component_type> entity::get_component()
    {
        return { {}, entity{data} };
    }

    template<typename component_type>
    inline const component<component_type> entity::get_component() const
    {
        return { {}, entity{data} };
    }

    template<typename component_type>
    inline void entity::remove_component()
    {
        Registry::destroyComponent<component_type>(*this);
    }

}
