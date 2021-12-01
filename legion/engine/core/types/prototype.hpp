#pragma once
#include <core/types/reflector.hpp>

namespace legion::core
{
    namespace detail
    {
        template<typename T, typename... MemberTypes>
        struct prototype_impl
        {
            using source_type = T;

            static constexpr size_type size = sizeof...(MemberTypes);
            std::tuple<MemberTypes...> values;
            std::array<std::string, sizeof...(MemberTypes)> names;

            constexpr prototype_impl() noexcept = default;
            template<typename Type, typename... Members>
            constexpr prototype_impl(const Type&, const std::array<std::string, sizeof...(MemberTypes)>& memberNames, Members&&... members) noexcept
                : values(std::forward<Members>(members)...), names(memberNames)
            {
                static_assert(std::is_same_v<Type, T>, "Prototype initialized with wrong input type.");
            }
        };

        template<size_t I, typename T, typename Refl, typename... MemberTypes>
        constexpr auto extract_from_reflector(const T& val, Refl&& refl, MemberTypes&&... members)
        {
            if constexpr (I == 0)
            {
                return prototype_impl<T, remove_cvr_t<MemberTypes>...>(val, refl.names, std::forward<MemberTypes>(members)...);
            }
            else
            {
                return extract_from_reflector<I - 1>(val, std::forward<Refl>(refl), std::get<I - 1>(refl.values), std::forward<MemberTypes>(members)...);
            }
        }

        template<typename T>
        constexpr auto get_prototype_impl(const T& val) noexcept
        {
            auto refl = make_reflector(val);
            return extract_from_reflector<refl.size>(val, refl);
        }

        template<typename T>
        using prototype_base = decltype(get_prototype_impl(std::declval<T>()));
    }

    template<typename T>
    struct prototype : public detail::prototype_base<T>
    {
        constexpr prototype(const T& val) noexcept : detail::prototype_base<T>(detail::get_prototype_impl(val)) {}
    };

    template<typename T>
    constexpr auto make_prototype(const T& val) noexcept
    {
        return prototype<T>(val);
    }
}
