#define STBI_MAX_DIMENSIONS 16384
#define STBI_NO_GIF
#define STB_IMAGE_IMPLEMENTATION
#if !defined(DOXY_EXCLUDE)
#include <stb_image.h>
#endif

#include <core/data/loaders/stbimageloader.hpp>

namespace legion::core
{
    using base = assets::AssetLoader<image>;
    using asset_ptr = base::asset_ptr;
    using import_cfg = base::import_cfg;
    using progress_type = base::progress_type;

    namespace detail
    {
        static bool stbi_test(const byte* ptr, size_type size)
        {
            stbi__context s;
            stbi__start_mem(&s, ptr, static_cast<int>(size));

            // test the formats with a very explicit header first (at least a FOURCC
            // or distinctive magic number first)
            return
#ifndef STBI_NO_PNG
                stbi__png_test(&s) ||
#endif
#ifndef STBI_NO_BMP
                stbi__bmp_test(&s) ||
#endif
#ifndef STBI_NO_GIF
                stbi__gif_test(&s) ||
#endif
#ifndef STBI_NO_PSD
                stbi__psd_test(&s) ||
#endif
#ifndef STBI_NO_PIC
                stbi__pic_test(&s) ||
#endif

                // then the formats that can end up attempting to load with just 1 or 2
                // bytes matching expectations; these are prone to false positives, so
                // try them later
#ifndef STBI_NO_JPEG
                stbi__jpeg_test(&s) ||
#endif
#ifndef STBI_NO_PNM
                stbi__pnm_test(&s) ||
#endif

#ifndef STBI_NO_HDR
                stbi__hdr_test(&s) ||
#endif

#ifndef STBI_NO_TGA
                // test tga last because it's a crappy test!
                stbi__tga_test(&s) ||
#endif
                false;
        }

        static data_view<byte> load_8bit(const byte_vec& data, math::ivec2& size, image_components& components, const import_cfg& settings)
        {
            byte* resultData = stbi_load_from_memory(
                data.data(), static_cast<int>(data.size()),
                &size.x, &size.y,
                reinterpret_cast<int*>(&components),
                settings.detectComponents ? static_cast<int>(settings.components) : 0);

            if (!settings.detectComponents)
                components = settings.components;

            auto dataSize = static_cast<size_type>(size.x * size.y) * static_cast<size_type>(components) * sizeof(byte);

            return { resultData, dataSize };
        }

        static data_view<byte> load_16bit(const byte_vec& data, math::ivec2& size, image_components& components, const import_cfg& settings)
        {
            uint16* resultData = stbi_load_16_from_memory(
                data.data(), static_cast<int>(data.size()),
                &size.x, &size.y,
                reinterpret_cast<int*>(&components),
                settings.detectComponents ? static_cast<int>(settings.components) : 0);

            if (!settings.detectComponents)
                components = settings.components;

            auto dataSize = static_cast<size_type>(size.x * size.y) * static_cast<size_type>(components) * sizeof(uint16);

            return { reinterpret_cast<byte*>(resultData), dataSize };
        }

        static data_view<byte> load_hdr(const byte_vec& data, math::ivec2& size, image_components& components, const import_cfg& settings)
        {
            float* resultData = stbi_loadf_from_memory(
                data.data(), static_cast<int>(data.size()),
                &size.x, &size.y,
                reinterpret_cast<int*>(&components),
                settings.detectComponents ? static_cast<int>(settings.components) : 0);

            if (!settings.detectComponents)
                components = settings.components;

            auto dataSize = static_cast<size_type>(size.x * size.y) * static_cast<size_type>(components) * sizeof(float);

            return { reinterpret_cast<byte*>(resultData), dataSize };
        }
    }

    bool StbImageLoader::canLoad(const fs::view& file)
    {
        auto result = file.get();
        if (!result)
        {
            result.mark_handled();
            return false;
        }

        return detail::stbi_test(result->data(), result->size());
    }

    common::result<asset_ptr> StbImageLoader::load(id_type nameHash, const fs::view& file, const import_cfg& settings)
    {
        OPTICK_EVENT();

        auto result = file.get();
        if (!result)
            return { legion_exception_msg(result.error().what()), result.warnings() };

        // Prefetch data from the resource.
        const byte_vec& data = result->get();

        // Setup stb_image settings.
        stbi_set_flip_vertically_on_load(settings.flipVertical);

        math::ivec2 size;

        image_components components = image_components::grey;
        channel_format format;

        data_view<byte> imageData;

        if (settings.detectFormat)
        {
            if (stbi_is_16_bit_from_memory(data.data(), static_cast<int>(data.size())))
            {
                format = channel_format::sixteen_bit;
                imageData = detail::load_16bit(data, size, components, settings);
            }
            else if (stbi_is_hdr_from_memory(data.data(), static_cast<int>(data.size())))
            {
                format = channel_format::float_hdr;
                imageData = detail::load_hdr(data, size, components, settings);
            }
            else
            {
                format = channel_format::eight_bit;
                imageData = detail::load_8bit(data, size, components, settings);
            }
        }
        else
        {
            format = settings.fileFormat;

            switch (settings.fileFormat)
            {
            default:
            case channel_format::eight_bit:
                imageData = detail::load_8bit(data, size, components, settings);
                break;
            case channel_format::sixteen_bit:
                imageData = detail::load_16bit(data, size, components, settings);
                break;
            case channel_format::float_hdr:
                imageData = detail::load_hdr(data, size, components, settings);
                break;
            case channel_format::depth_stencil:
                return legion_fs_error("invalid channel format");
            }
        }

        return create(nameHash, size, format, components, imageData);
    }

    void StbImageLoader::free(image& asset)
    {
        stbi_image_free(asset.data());
    }
}
