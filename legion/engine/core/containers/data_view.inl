#include <core/containers/data_view.hpp>
#pragma once

namespace legion::core
{
    template<typename DataType>
    inline L_ALWAYS_INLINE data_view<DataType>::data_view() noexcept
        : data_view(nullptr, 0, 0) {}

    template<typename DataType>
    inline L_ALWAYS_INLINE data_view<DataType>::data_view(std::nullptr_t) noexcept
        : data_view(nullptr, 0, 0) {}

    template<typename DataType>
    inline data_view<DataType>::data_view(std::vector<value_type>& vec, diff_type offset)
        : data_view(vec.data(), vec.size() - offset, offset)
    {
        if (offset > vec.size()) throw std::out_of_range("data_view constructed with out of range offset");
    }

    template<typename DataType>
    template<size_type N>
    inline core::data_view<DataType>::data_view(std::array<value_type, N>& arr, diff_type offset)
        : data_view(arr.data(), arr.size() - offset, offset)
    {
        if (offset > arr.size()) throw std::out_of_range("data_view constructed with out of range offset");
    }

    template<typename DataType>
    template<typename Traits, typename Allocator>
    inline data_view<DataType>::data_view(std::basic_string<DataType, Traits, Allocator>& str, diff_type offset)
        : data_view(str.data(), str.size() - offset, offset)
    {
        if (offset > str.size()) throw std::out_of_range("data_view constructed with out of range offset");
    }


    template<typename DataType>
    inline data_view<DataType>::data_view(ptr_type ptr, size_type size, diff_type offset) noexcept
        : m_data(ptr), m_offset(offset), m_size(size) {}

    template<typename DataType>
    inline data_view<DataType>::data_view(const data_view<DataType>& other) noexcept
        : m_data(other.m_data), m_offset(other.m_offset), m_size(other.m_size) {}

    template<typename DataType>
    inline data_view<DataType>::data_view(data_view<DataType>&& other) noexcept
        : m_data(std::move(other.m_data)), m_offset(std::move(other.m_offset)), m_size(std::move(other.m_size)) {}

    template<typename DataType>
    inline data_view<DataType>& data_view<DataType>::operator=(const data_view<DataType>& other) noexcept
    {
        m_data = other.m_data;
        m_offset = other.m_offset;
        m_size = other.m_size;

        return *this;
    }

    template<typename DataType>
    inline data_view<DataType>& data_view<DataType>::operator=(data_view<DataType>&& other) noexcept
    {
        m_data = std::move(other.m_data);
        m_offset = std::move(other.m_offset);
        m_size = std::move(other.m_size);

        return *this;
    }

    template<typename DataType>
    inline data_view<DataType>::operator bool() const noexcept
    {
        return m_data;
    }

    template<typename DataType>
    inline bool data_view<DataType>::operator==(const data_view& other) const noexcept
    {
        return m_data == other.m_data && m_offset == other.m_offset && m_size == other.m_size;
    }

    template<typename DataType>
    inline L_ALWAYS_INLINE bool data_view<DataType>::operator!=(const data_view& other) const noexcept
    {
        return !operator==(other);
    }

    template<typename DataType>
    inline DataType& data_view<DataType>::at(size_type idx)
    {
        if (idx > m_size) throw std::out_of_range("data_view subscript out of range");
        return *(m_data + m_offset + static_cast<ptrdiff_t>(idx));
    }

    template<typename DataType>
    inline const DataType& data_view<DataType>::at(size_type idx) const
    {
        if (idx > m_size) throw std::out_of_range("data_view subscript out of range");
        return *(m_data + m_offset + static_cast<ptrdiff_t>(idx));
    }

    template<typename DataType>
    inline L_ALWAYS_INLINE DataType& data_view<DataType>::operator[](size_type idx)
    {
        return at(idx);
    }

    template<typename DataType>
    inline L_ALWAYS_INLINE const DataType& data_view<DataType>::operator[](size_type idx) const
    {
        return at(idx);
    }

    template<typename DataType>
    inline typename data_view<DataType>::iterator data_view<DataType>::begin() noexcept
    {
        return m_data + m_offset;
    }

    template<typename DataType>
    inline typename data_view<DataType>::iterator data_view<DataType>::end() noexcept
    {
        return begin() + m_size;
    }

    template<typename DataType>
    inline L_ALWAYS_INLINE DataType* data_view<DataType>::data() noexcept
    {
        return begin();
    }

    template<typename DataType>
    inline typename data_view<DataType>::const_iterator data_view<DataType>::begin() const noexcept
    {
        return m_data + m_offset;
    }

    template<typename DataType>
    inline typename data_view<DataType>::const_iterator data_view<DataType>::end() const noexcept
    {
        return begin() + m_size;
    }

    template<typename DataType>
    inline L_ALWAYS_INLINE const DataType* data_view<DataType>::data() const noexcept
    {
        return begin();
    }

    template<typename DataType>
    inline L_ALWAYS_INLINE size_type data_view<DataType>::size() const noexcept
    {
        return m_size;
    }

    template<typename DataType>
    inline ptrdiff_t data_view<DataType>::offset() const noexcept
    {
        return m_offset;
    }

    template<typename DataType>
    inline L_ALWAYS_INLINE size_type data_view<DataType>::max_size() const noexcept
    {
        return m_size;
    }
}
