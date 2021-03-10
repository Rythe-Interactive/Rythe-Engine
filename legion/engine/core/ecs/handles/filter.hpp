#pragma once

namespace legion::core::ecs
{
    template<typename... component_types>
    struct filter
    {
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

    };

}
