#include <core/common/hash.hpp>

namespace legion::core
{
    constexpr hash::operator id_type() const noexcept
    {
        return value;
    }

    constexpr id_type combine_hash(id_type seed, id_type value)
    {
        value += 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed ^ value;
    }

    id_type local_cast(id_type global)
    {
        return detail::global_to_local.at(global);
    }

    id_type global_cast(id_type local)
    {
        return detail::local_to_global.at(local);
    }

    type_reference type_ref_cast(id_type hash)
    {
        return detail::hash_to_reference.at(hash);
    }

    constexpr name_hash::name_hash() noexcept {}

    constexpr name_hash::name_hash(id_type value) noexcept { this->value = value; }

    name_hash::name_hash(const name_hash& src) noexcept { this->value = value; }

    name_hash::name_hash(name_hash&& src) noexcept { this->value = value; }

    name_hash& name_hash::operator=(const name_hash& src) noexcept
    {
        value = src.value;
        return *this;
    }

    name_hash& name_hash::operator=(name_hash&& src) noexcept
    {
        value = src.value;
        return *this;
    }

    constexpr id_type type_hash_base::local() const noexcept
    {
        return value;
    }

    constexpr std::string_view type_hash_base::local_name() const noexcept
    {
        return name;
    }

    type_reference::type_reference(std::nullptr_t)
        : value(nullptr) {}

    type_reference::type_reference(const type_hash_base& src)
        : value(src.copy())
    {
        detail::hash_to_reference.emplace(value->value, *this);
    }

    type_reference::type_reference(const type_reference& src)
        : value(src.value->copy()) {}

    type_reference::type_reference(type_reference&& src)
        : value(std::move(src.value)) {}

    type_reference& type_reference::operator=(const type_reference& src)
    {
        value = std::unique_ptr<type_hash_base>(src.value->copy());
        return *this;
    }

    type_reference& type_reference::operator=(type_reference&& src)
    {
        value = std::move(src.value);
        return *this;
    }

    id_type type_reference::local() const
    {
        return value->value;
    }

    id_type type_reference::global() const
    {
        return value->global();
    }

    std::string_view type_reference::local_name() const
    {
        return value->name;
    }

    std::string_view type_reference::global_name() const
    {
        return value->global_name();
    }

    type_reference::operator id_type() const
    {
        return value->value;
    }

    constexpr name_hash literals::operator""_hs(cstring src)
    {
        return name_hash{ nameHash(src) };
    }
}
