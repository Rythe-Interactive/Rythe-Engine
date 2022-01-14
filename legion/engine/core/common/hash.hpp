#pragma once
#include <unordered_map>
#include <string_view>

#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/common/string_literal.hpp>
#include <core/engine/enginesubsystem.hpp>
#include <core/engine/engine.hpp>

namespace legion::core
{
    constexpr id_type combine_hash(id_type seed, id_type value);

    struct name_hash
    {
        id_type value = invalid_id;

        constexpr operator id_type () const noexcept;

        constexpr name_hash() noexcept;

        template<size_type N>
        constexpr name_hash(const char(&name)[N]) noexcept;

        template<size_type N>
        constexpr name_hash(const string_literal<N>& name) noexcept;

        constexpr name_hash(id_type value) noexcept;

        name_hash(const name_hash& src) noexcept;
        name_hash(name_hash&& src) noexcept;

        name_hash& operator=(const name_hash& src) noexcept;
        name_hash& operator=(name_hash&& src) noexcept;
    };

    namespace detail
    {
        struct type_data : public EngineSubSystem<type_data>
        {
            std::unordered_map<id_type, std::string> id_to_name;

            AllowPrivateOnInit;
            SubSystemInstance(type_data);
        private:
            static void onInit();
        };

        OnEngineInit(type_data, &type_data::init);
        OnEngineShutdown(type_data, &type_data::shutdown);

        template<typename T>
        std::string_view register_name(id_type id)
        {
            auto [iterator, emplaced] = type_data::getInstance().id_to_name.emplace(id, std::string(nameOfType<T>()));
            return iterator->second;
        }
    }

    struct type_hash
    {
        template<typename T>
        friend type_hash make_hash() noexcept;

        L_NODISCARD static type_hash from_name(std::string_view name);
        L_NODISCARD static type_hash from_id(id_type id);

        type_hash(const type_hash& src) noexcept;
        type_hash(type_hash&& src) noexcept;

        type_hash& operator=(const type_hash& src) noexcept;

        type_hash& operator=(type_hash&& src) noexcept;

        operator id_type() const noexcept;

        L_NODISCARD id_type id() const noexcept;
        L_NODISCARD std::string_view name() const noexcept;

    protected:
        type_hash(id_type id, std::string_view name) noexcept;

        std::string_view m_name;
        id_type m_value = invalid_id;
    };

    template<typename T>
    type_hash make_hash() noexcept
    {
        static id_type id = typeHash<T>();
        static std::string_view name = detail::register_name<T>(id);

        return type_hash{ id, name };
    }

    template<size_type N>
    constexpr name_hash make_hash(const string_literal<N>& src) noexcept
    {
        return name_hash{ src };
    }

    namespace literals
    {
        constexpr name_hash operator""_hs(cstring src);
    }

}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<> struct hash<legion::core::type_hash>
    {
        std::size_t operator()(legion::core::type_hash const& handle) const noexcept
        {
            return handle.id();
        }
    };
}
#endif

#include <core/common/hash.inl>
