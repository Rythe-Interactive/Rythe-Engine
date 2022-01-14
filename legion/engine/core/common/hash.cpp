#include <core/common/hash.hpp>

namespace legion::core
{
    namespace detail
    {
        void type_data::onInit()
        {
            create();
        }
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

    type_hash::type_hash(const type_hash& src) noexcept : m_name(src.m_name), m_value(src.m_value) {}

    type_hash::type_hash(type_hash&& src) noexcept : m_name(src.m_name), m_value(src.m_value) {}

    type_hash& type_hash::operator=(const type_hash& src) noexcept
    {
        m_value = src.m_value;
        m_name = src.m_name;
        return *this;
    }

    type_hash& type_hash::operator=(type_hash&& src) noexcept
    {
        m_value = src.m_value;
        m_name = src.m_name;
        return *this;
    }

    type_hash::operator id_type() const noexcept { return id(); }

    L_NODISCARD id_type type_hash::id() const noexcept { return m_value; }

    L_NODISCARD std::string_view type_hash::name() const noexcept { return m_name; }

    type_hash::type_hash(id_type id, std::string_view name) noexcept : m_name(name), m_value(id) {}

    L_NODISCARD type_hash type_hash::from_name(std::string_view name)
    {
        return type_hash(nameHash(name), name);
    }

    L_NODISCARD type_hash type_hash::from_id(id_type id)
    {
        return type_hash(id, detail::type_data::getInstance().id_to_name[id]);
    }
}
