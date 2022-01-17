#pragma once
#include <core/platform/platform.hpp>
#include <core/math/math.hpp>
#include <core/common/hash.hpp>

#include <core/ecs/handles/entity.hpp>

/**
 * @file component.hpp
 */

namespace legion::core::ecs
{
    /**@class component_base
     * @brief Shared bass class for component handles.
     */
    struct component_base
    {
    };

    /**@class component
     * @brief Inter frame storable handle to a component. 
     * @tparam component_type Type of component the handle references.
     */
    template<typename component_type>
    struct component : public component_base
    {
        static const type_hash typeId;

        entity owner;

        component& operator=(const component_type& src);
        component& operator=(component_type&& src);

        L_NODISCARD operator component_type& ();
        L_NODISCARD operator const component_type& () const;

        L_NODISCARD bool valid() const noexcept;
        L_NODISCARD operator bool() const noexcept;

        L_NODISCARD component_type& operator*();
        L_NODISCARD const component_type& operator*() const;

        L_NODISCARD component_type* operator->();
        L_NODISCARD const component_type* operator->() const;

        bool operator==(const component& other) const noexcept;

        /**@brief Gets a reference to the component the handle references.
         */
        L_NODISCARD component_type& get();
        L_NODISCARD const component_type& get() const;

        /**@brief Destroys the component the handle references.
         */
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
                std::hash<legion::core::id_type>{}(handle.owner->id));
            legion::core::math::detail::hash_combine(hash,
                legion::core::typeHash<component_type>());

            return hash;
        }
    };
}
#endif
