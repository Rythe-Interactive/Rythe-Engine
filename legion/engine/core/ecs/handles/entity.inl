#include <core/ecs/handles/entity.hpp>
#include <core/ecs/registry.hpp>
#pragma once

#if !defined(DOXY_EXCLUDE)
namespace std
{
    inline L_ALWAYS_INLINE size_t hash<legion::core::ecs::entity>::operator()(legion::core::ecs::entity const& handle) const noexcept
    {
        return handle->id;
    }
}
#endif

namespace legion::core::ecs
{
    namespace detail
    {
        template<typename component_type>
        inline L_ALWAYS_INLINE auto _wrap_in_tuple(entity target)
        {
            if constexpr (maybe_component_v<component_type>)
                return std::make_tuple(component<remove_cvr_t<component_type>>{ {}, target });
            else
                return std::make_tuple(component_type::get_handles(target));
        }

        template<typename component_type>
        inline L_ALWAYS_INLINE auto _get_handle(entity target)
        {
            if constexpr (maybe_component_v<component_type>)
                return component<remove_cvr_t<component_type>>{ {}, target };
            else
                return component_type::get_handles(target);
        }

        template<typename... component_types>
        inline L_ALWAYS_INLINE auto _get_handles(entity target)
        {
            return std::tuple_cat(_wrap_in_tuple<component_types>(target)...);
        }
    }

    template<>
    inline L_ALWAYS_INLINE bool entity::operator==<std::nullptr_t>(std::nullptr_t) const
    {
        return !(data && data->alive);
    }

    template<>
    inline L_ALWAYS_INLINE bool entity::operator!=<std::nullptr_t>(std::nullptr_t) const
    {
        return data && data->alive;
    }

    template<>
    inline L_ALWAYS_INLINE bool entity::operator==<id_type>(id_type id) const
    {
        return data && data->alive ? data->id == id : id == invalid_id;
    }

    template<>
    inline L_ALWAYS_INLINE bool entity::operator!=<id_type>(id_type id) const
    {
        return data && data->alive && (data->id != id || id == invalid_id);
    }

    template<>
    inline L_ALWAYS_INLINE bool entity::operator==<entity>(entity other) const
    {
        return data && data->alive && other.data && data->id == other->id;
    }

    template<>
    inline L_ALWAYS_INLINE bool entity::operator!=<entity>(entity other) const
    {
        return data && data->alive && other.data && data->id != other->id;
    }

    template<typename T>
    inline L_ALWAYS_INLINE bool entity::operator==(T val) const
    {
        return data && data->alive ? data->id == val : val == invalid_id;
    }

    template<typename T>
    inline L_ALWAYS_INLINE bool entity::operator!=(T val) const
    {
        return data && data->alive && (data->id != val || val == invalid_id);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE wrap_component_t<component_type> entity::add_component()
    {
        Registry::createComponent<component_type>(*this);
        return detail::_get_handle<component_type>(*this);
    }

    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline L_ALWAYS_INLINE component_tuple<component_type0, component_type1, component_typeN...> entity::add_component()
    {
        Registry::createComponent<component_type0, component_type1, component_typeN...>(*this);
        return detail::_get_handles<component_type0, component_type1, component_typeN...>(*this);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE wrap_component_t<component_type> entity::add_component(component_type&& value)
    {
        Registry::createComponent<remove_cvr_t<component_type>>(*this, std::forward<component_type>(value));
        return detail::_get_handle<component_type>(*this);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE wrap_component_t<component_type> entity::add_component(const component_type& value)
    {
        Registry::createComponent<component_type>(*this, value);
        return detail::_get_handle<component_type>(*this);
    }

    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline L_ALWAYS_INLINE component_tuple<component_type0, component_type1, component_typeN...> entity::add_component(component_type0&& value0, component_type1&& value1, component_typeN&&... valueN)
    {
        Registry::createComponent<remove_cvr_t<component_type0>, remove_cvr_t<component_type1>, remove_cvr_t<component_typeN>...>(*this, std::forward<component_type0>(value0), std::forward<component_type1>(value1), std::forward<component_typeN>(valueN)...);
        return detail::_get_handles<component_type0, component_type1, component_typeN...>(*this);
    }

    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline L_ALWAYS_INLINE component_tuple<component_type0, component_type1, component_typeN...> entity::add_component(const component_type0& value0, const component_type1& value1, const component_typeN&... valueN)
    {
        Registry::createComponent<component_type0, component_type1, component_typeN...>(*this, value0, value1, valueN...);
        return detail::_get_handles<component_type0, component_type1, component_typeN...>(*this);
    }

    template<typename archetype_type, typename component_type0, typename component_type1, typename... component_typeN>
    inline L_ALWAYS_INLINE typename archetype_type::handle_group entity::add_component(component_type0&& value0, component_type1&& value1, component_typeN&&... valueN)
    {
        Registry::createComponent<archetype_type>(*this, std::forward<component_type0>(value0), std::forward<component_type1>(value1), std::forward<component_typeN>(valueN)...);
        return std::make_tuple(component<remove_cvr_t<component_type0>>{ {}, entity{ data } }, component<remove_cvr_t<component_type1>>{ {}, entity{ data } }, component<remove_cvr_t<component_typeN>>{ {}, entity{ data } }...);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component<component_type> entity::add_component(const serialization::prototype<component<component_type>>& prot)
    {
        Registry::createComponent<component_type>(*this, prot);
        return { {}, entity{data} };
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component<component_type> entity::add_component(serialization::prototype<component<component_type>>&& prot)
    {
        Registry::createComponent<component_type>(*this, std::move(prot));
        return { {}, entity{data} };
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE bool entity::has_component() const
    {
        return Registry::hasComponent<component_type>(*this);
    }

    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline L_ALWAYS_INLINE bool entity::has_component() const
    {
        return Registry::hasComponent<component_type0, component_type1, component_typeN...>(*this);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE wrap_component_t<component_type> entity::get_component()
    {
        if constexpr (is_archetype_v<component_type>)
        {
            return component_type::get_handles(*this);
        }
        else
        {
            return { {}, entity{data} };
        }
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE const_wrap_component_t<component_type> entity::get_component() const
    {
        if constexpr (is_archetype_v<component_type>)
        {
            return component_type::get_const_handles(*this);
        }
        else
        {
            return { {}, entity{data} };
        }
    }

    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline L_ALWAYS_INLINE component_tuple<component_type0, component_type1, component_typeN...> entity::get_component()
    {
        return std::make_tuple(get_component<component_type0>(), get_component<component_type1>(), get_component<component_typeN>()...);
    }

    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline L_ALWAYS_INLINE const_component_tuple<component_type0, component_type1, component_typeN...> entity::get_component() const
    {
        return std::make_tuple(get_component<component_type0>(), get_component<component_type1>(), get_component<component_typeN>()...);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void entity::remove_component()
    {
        Registry::destroyComponent<component_type>(*this);
    }

    template<typename component_type0, typename component_type1, typename... component_typeN>
    inline L_ALWAYS_INLINE void entity::remove_component()
    {
        Registry::destroyComponent<component_type0, component_type1, component_typeN...>(*this);
    }

}
