#pragma once
#include <core/types/types.hpp>
#include <vector>

namespace legion::core::ecs
{
    template<typename component_type>
    struct component_container;

    struct component_container_base
    {
    public:
        id_type m_componentid = invalid_id;

    protected:
        component_container_base(id_type componentId) noexcept : m_componentid(componentId) {}

        id_type getComponentTypeId() const noexcept { return m_componentid; }

    public:
        component_container_base() = default;

        template<typename component_type>
        component_container<component_type>& cast() noexcept;

        template<typename component_type>
        const component_container<component_type>& cast() const noexcept;
    };

    /**@class component_container
     * @brief This is just a vector with a common base class.
     */
    template<typename component_type>
    struct component_container : public component_container_base, public std::vector<component_type>
    {
        using underlying_type = std::vector<component_type>;
        using allocator_type = typename underlying_type::allocator_type;

        component_container() noexcept : component_container_base(typeHash<component_type>()), underlying_type() {}

        explicit component_container(const allocator_type& alloc) noexcept
            : component_container_base(typeHash<component_type>()), underlying_type(alloc) {}

        component_container(size_type count,
            const component_type& value,
            const allocator_type& alloc = allocator_type())
            : component_container_base(typeHash<component_type>()), underlying_type(count, value, alloc) {}

        explicit component_container(size_type count, const allocator_type& alloc = allocator_type())
            : component_container_base(typeHash<component_type>()), underlying_type(count, alloc) {}

        template<typename InputIt>
        component_container(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
            : component_container_base(typeHash<component_type>()), underlying_type(first, last, alloc) {}

        component_container(const underlying_type& other)
            : component_container_base(typeHash<component_type>()), underlying_type(other) {}
        component_container(const component_container& other)
            : component_container_base(typeHash<component_type>()), underlying_type(static_cast<const underlying_type&>(other)) {}

        component_container(const underlying_type& other, const allocator_type& alloc)
            : component_container_base(typeHash<component_type>()), underlying_type(other, alloc) {}
        component_container(const component_container& other, const allocator_type& alloc)
            : component_container_base(typeHash<component_type>()), underlying_type(static_cast<const underlying_type&>(other), alloc) {}

        component_container(underlying_type&& other) noexcept
            : component_container_base(typeHash<component_type>()), underlying_type(other) {}
        component_container(component_container&& other) noexcept
            : component_container_base(typeHash<component_type>()), underlying_type(static_cast<underlying_type&&>(other)) {}

        component_container(underlying_type&& other, const allocator_type& alloc)
            : component_container_base(typeHash<component_type>()), underlying_type(other, alloc) {}
        component_container(component_container&& other, const allocator_type& alloc)
            : component_container_base(typeHash<component_type>()), underlying_type(static_cast<underlying_type&&>(other), alloc) {}

        component_container(std::initializer_list<component_type> init,
            const allocator_type& alloc = allocator_type())
            : component_container_base(typeHash<component_type>()), underlying_type(init, alloc) {}

    };

    template<>
    struct component_container<void> : public component_container_base, public std::vector<std::nullptr_t>
    {
        using underlying_type = std::vector<std::nullptr_t>;
        using allocator_type = typename underlying_type::allocator_type;
    };

    template<>
    struct component_container<std::nullptr_t> : public component_container_base, public std::vector<std::nullptr_t>
    {
        using underlying_type = std::vector<std::nullptr_t>;
        using allocator_type = typename underlying_type::allocator_type;
    };

    static inline component_container<void> invalid_container;

    template<typename component_type>
    inline component_container<component_type>& component_container_base::cast() noexcept
    {
        if (typeHash<component_type>() != m_componentid)
            return *reinterpret_cast<component_container<component_type>*>(&invalid_container);;
        return *static_cast<component_container<component_type>*>(this);
    }

    template<typename component_type>
    inline const component_container<component_type>& component_container_base::cast() const noexcept
    {
        if (typeHash<component_type>() != m_componentid)
            return *reinterpret_cast<const component_container<component_type>*>(&invalid_container);;
        return *static_cast<const component_container<component_type>*>(this);
    }
}
