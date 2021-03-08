#include <core/ecs/handles/entity.hpp>
#include <core/ecs/registry.hpp>
#pragma once


namespace legion::core::ecs
{
    template<typename component_type>
    inline component<component_type> entity::add_component()
    {
        Registry::createComponent<component_type>(*this);
        return { {}, entity{data} };
    }

    template<typename component_type>
    inline component<component_type> entity::add_component(const serialization::component_prototype<component_type>& prot)
    {
        Registry::createComponent<component_type>(*this, prot);
        return { {}, entity{data} };
    }

    template<typename component_type>
    inline component<component_type> entity::add_component(serialization::component_prototype<component_type>&& prot)
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

#if !defined(DOXY_EXCLUDE)
namespace std
{
    constexpr size_t hash<legion::core::ecs::entity>::operator()(legion::core::ecs::entity const& handle) const noexcept
    {
        return handle->id;
    }
}
#endif
