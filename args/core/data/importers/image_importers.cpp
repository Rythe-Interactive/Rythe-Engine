#define STBI_MAX_DIMENSIONS 16384
#define STBI_NO_GIF
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <core/data/importers/image_importers.hpp>

namespace args::core
{
    common::result_decay_more<image, fs_error> stb_image_loader::load(const filesystem::basic_resource& resource, image_import_settings settings)
    {
        using common::Err, common::Ok;
        // Decay overloads the operator of ok_type and operator== for valid_t.
        using decay = common::result_decay_more<image, fs_error>;

        // Prefetch data from the resource.
        const byte_vec& data = resource.get();

        // Setup stb_image settings.
        stbi_set_flip_vertically_on_load(settings.flipVertical);

        // Create texture object and store the representation values.
        image image{};

        // Throwaway temporary storage for the original components in the texture that we're loading. (Everything gets converted to the components specified in the settings anyways.)
        image_components components = image_components::grey;

        // Pointer to the start of the data array created by stb_image. It needs to be void because it could either be filled with bytes, ushorts, or floats.
        void* imageData;

        // Load the image data using stb_image.
        switch (settings.fileFormat)
        {
        default: [[fallthrough]];
        case channel_format::eight_bit:
        {
            imageData = stbi_load_from_memory(data.data(), data.size(), &image.size.x, &image.size.y, reinterpret_cast<int*>(&components), static_cast<int>(settings.components));
            break;
        }
        case channel_format::sixteen_bit:
        {
            imageData = stbi_load_16_from_memory(data.data(), data.size(), &image.size.x, &image.size.y, reinterpret_cast<int*>(&components), static_cast<int>(settings.components));
            break;
        }
        case channel_format::float_hdr:
        {
            imageData = stbi_loadf_from_memory(data.data(), data.size(), &image.size.x, &image.size.y, reinterpret_cast<int*>(&components), static_cast<int>(settings.components));
            break;
        }
        }
    }
}
