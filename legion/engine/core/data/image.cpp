#include <core/data/image.hpp>
#include <core/filesystem/assetimporter.hpp>

namespace legion::core
{
    common::result<void> image::_apply_raw_impl() const
    {
        m_colors->reserve(static_cast<size_type>(m_resolution.x * m_resolution.y));

        size_type channelSize = static_cast<size_type>(m_format);
        size_type colorSize = static_cast<size_type>(m_components) * channelSize;

        byte* end = m_data.end();
        for (auto colorPtr = m_data.begin(); colorPtr < end; colorPtr += colorSize)
        {
            math::color color;

            switch (m_components)
            {
            case image_components::grey:
            {
                switch (m_format)
                {
                case channel_format::eight_bit:
                {
                    float grayValue = (*colorPtr / 255.f);
                    color.r = grayValue;
                    color.g = grayValue;
                    color.b = grayValue;
                    color.a = 1.f;
                    break;
                }
                case channel_format::sixteen_bit:
                {
                    float grayValue = (*reinterpret_cast<uint16*>(colorPtr) / 65535.f);
                    color.r = grayValue;
                    color.g = grayValue;
                    color.b = grayValue;
                    color.a = 1.f;
                    break;
                }
                case channel_format::float_hdr:
                {
                    float grayValue = *reinterpret_cast<float*>(colorPtr);
                    color.r = grayValue;
                    color.g = grayValue;
                    color.b = grayValue;
                    color.a = 1.f;
                    break;
                }
                case channel_format::depth_stencil:
                default:
                    return legion_exception_msg("invalid channel format");
                }
                break;
            }
            case image_components::grey_alpha:
            {
                byte* gPtr = colorPtr + channelSize * 0;
                byte* aPtr = colorPtr + channelSize * 1;

                switch (m_format)
                {
                case channel_format::eight_bit:
                {
                    float grayValue = (*gPtr / 255.f);
                    color.r = grayValue;
                    color.g = grayValue;
                    color.b = grayValue;
                    color.a = (*aPtr / 255.f);
                    break;
                }
                case channel_format::sixteen_bit:
                {
                    float grayValue = (*reinterpret_cast<uint16*>(gPtr) / 65535.f);
                    color.r = grayValue;
                    color.g = grayValue;
                    color.b = grayValue;
                    color.a = (*reinterpret_cast<uint16*>(aPtr) / 65535.f);
                    break;
                }
                case channel_format::float_hdr:
                {
                    float grayValue = *reinterpret_cast<float*>(gPtr);
                    color.r = grayValue;
                    color.g = grayValue;
                    color.b = grayValue;
                    color.a = *reinterpret_cast<float*>(aPtr);
                    break;
                }
                case channel_format::depth_stencil:
                default:
                    return legion_exception_msg("invalid channel format");
                }
                break;
            }
            case image_components::rgb:
            {
                byte* rPtr = colorPtr + channelSize * 0;
                byte* gPtr = colorPtr + channelSize * 1;
                byte* bPtr = colorPtr + channelSize * 2;

                switch (m_format)
                {
                case channel_format::eight_bit:
                {
                    color.r = (*rPtr / 255.f);
                    color.g = (*gPtr / 255.f);
                    color.b = (*bPtr / 255.f);
                    color.a = 1.f;
                    break;
                }
                case channel_format::sixteen_bit:
                {
                    color.r = (*reinterpret_cast<uint16*>(rPtr) / 65535.f);
                    color.g = (*reinterpret_cast<uint16*>(gPtr) / 65535.f);
                    color.b = (*reinterpret_cast<uint16*>(bPtr) / 65535.f);
                    color.a = 1.f;
                    break;
                }
                case channel_format::float_hdr:
                {
                    color.r = *reinterpret_cast<float*>(rPtr);
                    color.g = *reinterpret_cast<float*>(gPtr);
                    color.b = *reinterpret_cast<float*>(bPtr);
                    color.a = 1.f;
                    break;
                }
                case channel_format::depth_stencil:
                default:
                    return legion_exception_msg("invalid channel format");
                }
                break;
            }
            case image_components::rgba:
            {
                byte* rPtr = colorPtr + channelSize * 0;
                byte* gPtr = colorPtr + channelSize * 1;
                byte* bPtr = colorPtr + channelSize * 2;
                byte* aPtr = colorPtr + channelSize * 3;

                switch (m_format)
                {
                case channel_format::eight_bit:
                {
                    color.r = (*rPtr / 255.f);
                    color.g = (*gPtr / 255.f);
                    color.b = (*bPtr / 255.f);
                    color.a = (*aPtr / 255.f);
                    break;
                }
                case channel_format::sixteen_bit:
                {
                    color.r = (*reinterpret_cast<uint16*>(rPtr) / 65535.f);
                    color.g = (*reinterpret_cast<uint16*>(gPtr) / 65535.f);
                    color.b = (*reinterpret_cast<uint16*>(bPtr) / 65535.f);
                    color.a = (*reinterpret_cast<uint16*>(aPtr) / 65535.f);
                    break;
                }
                case channel_format::float_hdr:
                {
                    color.r = *reinterpret_cast<float*>(rPtr);
                    color.g = *reinterpret_cast<float*>(gPtr);
                    color.b = *reinterpret_cast<float*>(bPtr);
                    color.a = *reinterpret_cast<float*>(aPtr);
                    break;
                }
                case channel_format::depth_stencil:
                default:
                    return legion_exception_msg("invalid channel format");
                }
                break;
            }
            case image_components::depth:
            case image_components::depth_stencil:
            case image_components::stencil:
            default:
                return legion_exception_msg("invalid image components");
            }

            m_colors->push_back(color);
        }

        return common::success;
    }

    image::image(const math::ivec2& res, channel_format format, image_components comp, const data_view<byte>& data)
        : m_resolution(res), m_format(format), m_components(comp), m_data(data)
    {
    }

    const math::ivec2& image::resolution() const noexcept
    {
        return m_resolution;
    }

    const channel_format& image::format() const noexcept
    {
        return m_format;
    }

    const image_components& image::components() const noexcept
    {
        return m_components;
    }

    common::result<void> image::apply_raw(bool lazyApply)
    {
        m_colors->clear();

        if (lazyApply)
            return common::success;

        return _apply_raw_impl();
    }

    common::result<std::reference_wrapper<const std::vector<math::color>>> image::read_colors() const
    {
        OPTICK_EVENT();
        if (m_colors && m_colors->size())
            return std::cref(*m_colors);
        else if (!m_colors)
            m_colors.emplace();

        auto result = _apply_raw_impl();
        if (!result)
            return result.error();

        return std::cref(*m_colors);
    }

    common::result<void> image::write_colors(const std::vector<math::color>& colors)
    {
        auto channelSize = static_cast<size_type>(m_format);
        auto colorSize = static_cast<size_type>(m_components) * channelSize;
        auto dataSize = colorSize * colors.size();

        if (dataSize != m_data.size())
            return legion_exception_msg("Image size mismatch");

        byte* end = m_data.end();
        size_type i = static_cast<size_type>(0u);
        for (auto colorPtr = m_data.begin(); colorPtr < end; colorPtr += colorSize)
        {
            switch (m_components)
            {
            case image_components::grey:
            {
                switch (m_format)
                {
                case channel_format::eight_bit:
                {
                    *colorPtr = static_cast<byte>(math::iround(colors[i].r * 255.f));
                    break;
                }
                case channel_format::sixteen_bit:
                {
                    *reinterpret_cast<uint16*>(colorPtr) = static_cast<uint16>(math::iround(colors[i].r * 65535.f));
                    break;
                }
                case channel_format::float_hdr:
                {
                    *reinterpret_cast<float*>(colorPtr) = colors[i].r;
                    break;
                }
                case channel_format::depth_stencil:
                default:
                    return legion_exception_msg("invalid channel format");
                }
                break;
            }
            case image_components::grey_alpha:
            {
                byte* gPtr = colorPtr + channelSize * 0;
                byte* aPtr = colorPtr + channelSize * 1;

                switch (m_format)
                {
                case channel_format::eight_bit:
                {
                    *gPtr = static_cast<byte>(math::iround(colors[i].r * 255.f));
                    *aPtr = static_cast<byte>(math::iround(colors[i].a * 255.f));
                    break;
                }
                case channel_format::sixteen_bit:
                {
                    *reinterpret_cast<uint16*>(gPtr) = static_cast<uint16>(math::iround(colors[i].r * 65535.f));
                    *reinterpret_cast<uint16*>(aPtr) = static_cast<uint16>(math::iround(colors[i].a * 65535.f));
                    break;
                }
                case channel_format::float_hdr:
                {
                    *reinterpret_cast<float*>(gPtr) = colors[i].r;
                    *reinterpret_cast<float*>(aPtr) = colors[i].a;
                    break;
                }
                case channel_format::depth_stencil:
                default:
                    return legion_exception_msg("invalid channel format");
                }
                break;
            }
            case image_components::rgb:
            {
                byte* rPtr = colorPtr + channelSize * 0;
                byte* gPtr = colorPtr + channelSize * 1;
                byte* bPtr = colorPtr + channelSize * 2;

                switch (m_format)
                {
                case channel_format::eight_bit:
                {
                    *rPtr = static_cast<byte>(math::iround(colors[i].r * 255.f));
                    *gPtr = static_cast<byte>(math::iround(colors[i].g * 255.f));
                    *bPtr = static_cast<byte>(math::iround(colors[i].b * 255.f));
                    break;
                }
                case channel_format::sixteen_bit:
                {
                    *reinterpret_cast<uint16*>(rPtr) = static_cast<uint16>(math::iround(colors[i].r * 65535.f));
                    *reinterpret_cast<uint16*>(gPtr) = static_cast<uint16>(math::iround(colors[i].g * 65535.f));
                    *reinterpret_cast<uint16*>(bPtr) = static_cast<uint16>(math::iround(colors[i].b * 65535.f));
                    break;
                }
                case channel_format::float_hdr:
                {
                    *reinterpret_cast<float*>(rPtr) = colors[i].r;
                    *reinterpret_cast<float*>(gPtr) = colors[i].g;
                    *reinterpret_cast<float*>(bPtr) = colors[i].b;
                    break;
                }
                case channel_format::depth_stencil:
                default:
                    return legion_exception_msg("invalid channel format");
                }
                break;
            }
            case image_components::rgba:
            {
                byte* rPtr = colorPtr + channelSize * 0;
                byte* gPtr = colorPtr + channelSize * 1;
                byte* bPtr = colorPtr + channelSize * 2;
                byte* aPtr = colorPtr + channelSize * 3;

                switch (m_format)
                {
                case channel_format::eight_bit:
                {
                    *rPtr = static_cast<byte>(math::iround(colors[i].r * 255.f));
                    *gPtr = static_cast<byte>(math::iround(colors[i].g * 255.f));
                    *bPtr = static_cast<byte>(math::iround(colors[i].b * 255.f));
                    *aPtr = static_cast<byte>(math::iround(colors[i].a * 255.f));
                    break;
                }
                case channel_format::sixteen_bit:
                {
                    *reinterpret_cast<uint16*>(rPtr) = static_cast<uint16>(math::iround(colors[i].r * 65535.f));
                    *reinterpret_cast<uint16*>(gPtr) = static_cast<uint16>(math::iround(colors[i].g * 65535.f));
                    *reinterpret_cast<uint16*>(bPtr) = static_cast<uint16>(math::iround(colors[i].b * 65535.f));
                    *reinterpret_cast<uint16*>(aPtr) = static_cast<uint16>(math::iround(colors[i].a * 65535.f));
                    break;
                }
                case channel_format::float_hdr:
                {
                    *reinterpret_cast<float*>(rPtr) = colors[i].r;
                    *reinterpret_cast<float*>(gPtr) = colors[i].g;
                    *reinterpret_cast<float*>(bPtr) = colors[i].b;
                    *reinterpret_cast<float*>(aPtr) = colors[i].a;
                    break;
                }
                case channel_format::depth_stencil:
                default:
                    return legion_exception_msg("invalid channel format");
                }
                break;
            }
            case image_components::depth:
            case image_components::depth_stencil:
            case image_components::stencil:
            default:
                return legion_exception_msg("invalid image components");
            }
            i++;
        }

        if (m_colors)
            *m_colors = colors;
        else
            m_colors.emplace(colors);

        return common::success;
    }

    size_type image::data_size() const
    {
        return m_data.size();
    }

    bool image::operator==(const image& other) const noexcept
    {
        return m_data == other.m_data;
    }

    byte* image::data() noexcept
    {
        return m_data.data();
    }
}
