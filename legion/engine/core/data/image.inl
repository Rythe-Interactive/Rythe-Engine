#include <core/data/image.hpp>
#pragma once

namespace legion::core
{
    template<typename T>
    data_view<T> image::raw_data()
    {
        if (m_data.size() % sizeof(T) == 0)
            return { reinterpret_cast<T*>(m_data.data()), m_data.size() / sizeof(T) };
        return nullptr;
    }

    template<>
    inline data_view<void> image::raw_data<void>()
    {
        return { reinterpret_cast<void*>(m_data.data()) };
    }

    template<>
    inline data_view<byte> image::raw_data<byte>()
    {
        if (m_format == channel_format::eight_bit)
            return m_data;
        return nullptr;
    }

    template<>
    inline data_view<uint16> image::raw_data<uint16>()
    {
        if (m_format == channel_format::sixteen_bit && m_data.size() % sizeof(uint16) == 0)
            return { reinterpret_cast<uint16*>(m_data.data()), m_data.size() / sizeof(uint16) };
        return nullptr;
    }

    template<>
    inline data_view<float> image::raw_data<float>()
    {
        if (m_format == channel_format::float_hdr && m_data.size() % sizeof(float) == 0)
            return { reinterpret_cast<float*>(m_data.data()), m_data.size() / sizeof(float) };
        return nullptr;
    }

    template<typename T>
    const data_view<T> image::raw_data() const
    {
        if (m_data.size() % sizeof(T) == 0)
            return { reinterpret_cast<T*>(m_data.data()), m_data.size() / sizeof(T) };
        return nullptr;
    }

    template<>
    inline const data_view<void> image::raw_data<void>() const
    {
        return { reinterpret_cast<void*>(m_data.data()) };
    }

    template<>
    inline const data_view<byte> image::raw_data<byte>() const
    {
        if (m_format == channel_format::eight_bit)
            return { m_data.data(), m_data.size(), m_data.offset() };
        return nullptr;
    }

    template<>
    inline const data_view<uint16> image::raw_data<uint16>() const
    {
        if (m_format == channel_format::sixteen_bit && m_data.size() % sizeof(uint16) == 0)
            return { reinterpret_cast<uint16*>(m_data.data()), m_data.size() / sizeof(uint16) };
        return nullptr;
    }

    template<>
    inline const data_view<float> image::raw_data<float>() const
    {
        if (m_format == channel_format::float_hdr && m_data.size() % sizeof(float) == 0)
            return { reinterpret_cast<float*>(m_data.data()), m_data.size() / sizeof(float) };
        return nullptr;
    }
}
