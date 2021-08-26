#include <core/containers/shared_data_view.hpp>
#pragma once

namespace legion::core
{
    template<typename DataType>
    inline L_ALWAYS_INLINE shared_data_view<DataType>::shared_data_view(std::nullptr_t) noexcept
        : shared_data_view(nullptr, 0, 0, false) {}

    template<typename DataType>
    inline shared_data_view<DataType>::shared_data_view(ptr_type ptr, size_type size, diff_type offset) noexcept
        : m_data(ptr), m_offset(offset), m_size(size) {}

    template<typename DataType>
    inline shared_data_view<DataType>::shared_data_view(const shared_data_view<DataType>& other) noexcept
        : m_data(other.m_data), m_offset(other.m_offset), m_size(other.m_size) {}

    template<typename DataType>
    inline shared_data_view<DataType>::shared_data_view(shared_data_view<DataType>&& other) noexcept
        : m_data(std::move(other.m_data)), m_offset(std::move(other.m_offset)), m_size(std::move(other.m_size)) {}

    template<typename DataType>
    inline shared_data_view<DataType>& shared_data_view<DataType>::operator=(const shared_data_view<DataType>& other) noexcept
    {
        m_data = other.m_data;
        m_offset = other.m_offset;
        m_size = other.m_size;

        return *this;
    }

    template<typename DataType>
    inline shared_data_view<DataType>& shared_data_view<DataType>::operator=(shared_data_view<DataType>&& other) noexcept
    {
        m_data = std::move(other.m_data);
        m_offset = std::move(other.m_offset);
        m_size = std::move(other.m_size);

        return *this;
    }

    template<typename DataType>
    inline shared_data_view<DataType>::operator bool() const noexcept
    {
        return m_data ? true : false;
    }

    template<typename DataType>
    inline bool shared_data_view<DataType>::operator==(const shared_data_view& other) const noexcept
    {
        return m_data == other.m_data && m_offset == other.m_offset && m_size == other.m_size;
    }

    template<typename DataType>
    inline bool shared_data_view<DataType>::operator!=(const shared_data_view& other) const noexcept
    {
        return !operator==(other);
    }

    template<typename DataType>
    inline DataType& shared_data_view<DataType>::at(size_type idx)
    {
        if (idx > m_size) throw std::out_of_range("shared_data_view subscript out of range");
        return *(m_data.get() + m_offset + static_cast<ptrdiff_t>(idx));
    }

    template<typename DataType>
    inline const DataType& shared_data_view<DataType>::at(size_type idx) const
    {
        if (idx > m_size) throw std::out_of_range("shared_data_view subscript out of range");
        return *(m_data.get() + m_offset + static_cast<ptrdiff_t>(idx));
    }

    template<typename DataType>
    inline DataType& shared_data_view<DataType>::operator[](size_type idx)
    {
        return at(idx);
    }

    template<typename DataType>
    inline const DataType& shared_data_view<DataType>::operator[](size_type idx) const
    {
        return at(idx);
    }

    template<typename DataType>
    inline typename shared_data_view<DataType>::iterator shared_data_view<DataType>::begin() noexcept
    {
        return m_data.get() + m_offset;
    }

    template<typename DataType>
    inline typename shared_data_view<DataType>::iterator shared_data_view<DataType>::end() noexcept
    {
        return begin() + m_size;
    }

    template<typename DataType>
    inline DataType* shared_data_view<DataType>::data() noexcept
    {
        return begin();
    }

    template<typename DataType>
    typename shared_data_view<DataType>::const_iterator shared_data_view<DataType>::begin() const noexcept
    {
        return m_data.get() + m_offset;
    }

    template<typename DataType>
    typename shared_data_view<DataType>::const_iterator shared_data_view<DataType>::end() const noexcept
    {
        return begin() + m_size;
    }

    template<typename DataType>
    const DataType* shared_data_view<DataType>::data() const noexcept
    {
        return begin();
    }

    template<typename DataType>
    inline size_type shared_data_view<DataType>::size() const noexcept
    {
        return m_size;
    }

    template<typename DataType>
    inline diff_type shared_data_view<DataType>::offset() const noexcept
    {
        return m_offset;
    }

    template<typename DataType>
    inline size_type shared_data_view<DataType>::max_size() const noexcept
    {
        return m_size;
    }
}
