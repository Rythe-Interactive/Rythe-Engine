#pragma once
#include <core/platform/platform.hpp>
#include <core/math/math.hpp>

#include <core/common/hash.hpp>
#include <core/ecs/handles/entity.hpp>

namespace legion::core::ecs
{
    struct component_base
    {
        L_NODISCARD virtual type_reference type_id() const noexcept LEGION_PURE;
    };

    template<typename component_type>
    struct component : public component_base
    {
        static constexpr type_hash<component_type> typeId = make_hash<component_type>();
        L_NODISCARD virtual type_reference type_id() const noexcept { return typeId; };

        entity owner;

        component(const entity& src) : owner(src){}
        component(entity&& src) : owner(src){}

        RULE_OF_5(component)

        L_NODISCARD operator component_type& ();
        L_NODISCARD operator const component_type& () const;

        L_NODISCARD operator bool() const noexcept;

        L_NODISCARD component_type* operator->();
        L_NODISCARD const component_type* operator->() const;

        bool operator==(const component& other) const noexcept;

        L_NODISCARD component_type& get();
        L_NODISCARD const component_type& get() const;

        void destroy();
    };
}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<typename component_type>
    struct hash<legion::core::ecs::component<component_type>>
    {
        std::size_t operator()(legion::core::ecs::component<component_type> const& handle) const noexcept
        {
            std::size_t hash = 0;

            legion::core::math::detail::hash_combine(hash,
                std::hash<legion::core::id_type>{}(handle.entity));
            legion::core::math::detail::hash_combine(hash,
                legion::core::typeHash<component_type>());

            return hash;
        }
    };
}
#endif
