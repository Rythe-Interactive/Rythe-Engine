#include <core/common/hash.hpp>

namespace legion::core
{
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

    name_hash::name_hash(const name_hash& src) noexcept { value = src.value; }

    name_hash::name_hash(name_hash&& src) noexcept { value = src.value; }

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

    type_reference::type_reference(std::nullptr_t)
        : value(nullptr) {}

    type_reference::type_reference(const type_hash_base& src)
        : value(src.copy())
    {
        detail::hash_to_reference.emplace(value->local(), *this);
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
        return value->local();
    }

    id_type type_reference::global() const
    {
        return value->global();
    }

    std::string_view type_reference::local_name() const
    {
        return value->local_name();
    }

    std::string_view type_reference::global_name() const
    {
        return value->global_name();
    }

    type_reference::operator id_type() const
    {
        return value->local();
    }
}
