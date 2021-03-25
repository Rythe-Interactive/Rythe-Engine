#pragma once

#include <core/platform/platform.hpp>
#include <core/ecs/handles/entity.hpp>
#include <core/ecs/containers/component_container.hpp>

namespace legion::core::ecs
{
    template<typename... component_types>
    struct filter
    {
    private:
        std::tuple<component_container<component_types>...> m_containers;

    public:
        static const id_type id;

        L_NODISCARD static entity_set::iterator begin() noexcept;
        L_NODISCARD static entity_set::iterator end() noexcept;
        L_NODISCARD static entity_set::reverse_iterator rbegin() noexcept;
        L_NODISCARD static entity_set::reverse_iterator rend() noexcept;
        L_NODISCARD static entity_set::reverse_itr_range reverse_range() noexcept;

        L_NODISCARD static size_type size() noexcept;
        L_NODISCARD static bool empty() noexcept;

        L_NODISCARD static size_type count(entity val);
        L_NODISCARD static size_type contains(entity val);

        L_NODISCARD static entity_set::iterator find(entity val);

        L_NODISCARD static entity& at(size_type index);
        L_NODISCARD entity& operator[](size_type index);

        template<typename component_type>
        L_NODISCARD static component_container<component_type>& get()
        {
            return std::get<component_container<component_type>>(m_containers);
        }

    };

}
