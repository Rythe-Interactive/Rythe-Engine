#define STBI_MAX_DIMENSIONS 16384
#define STBI_NO_GIF
#include <stb_image.h>

#include <rendering/data/importers/texture_importers.hpp>

namespace legion::rendering
{
    common::result_decay_more<texture, fs_error> stbi_texture_loader::load(const fs::basic_resource& resource, texture_import_settings&& settings)
    {
        OPTICK_EVENT();
        using common::Err, common::Ok;
        // Decay overloads the operator of ok_type and operator== for valid_t.
        using decay = common::result_decay_more<texture, fs_error>;

        // Prefetch data from the resource.
        const byte_vec& data = resource.get();

        // Setup stb_image settings.
        stbi_set_flip_vertically_on_load(settings.flipVertical);

        // Create texture object and store the representation values.
        texture texture{};
        texture.channels = settings.components;
        texture.type = settings.type;
        math::ivec2 texSize;
        // Throwaway temporary storage for the original components in the texture that we're loading. (Everything gets converted to the components specified in the settings anyways.)
        texture_components components = texture_components::grey;

        // Pointer to the start of the data array created by stb_image. It needs to be void because it could either be filled with bytes, ushorts, or floats.
        void* imageData;

        // Load the image data using stb_image.
        switch (settings.fileFormat)
        {
            default: [[fallthrough]];
            case channel_format::eight_bit:
            {
                imageData = stbi_load_from_memory(data.data(), data.size(), &texSize.x, &texSize.y, reinterpret_cast<int*>(&components), static_cast<int>(settings.components));
                break;
            }
            case channel_format::sixteen_bit:
            {
                imageData = stbi_load_16_from_memory(data.data(), data.size(), &texSize.x, &texSize.y, reinterpret_cast<int*>(&components), static_cast<int>(settings.components));
                break;
            }
            case channel_format::float_hdr:
            {
                imageData = stbi_loadf_from_memory(data.data(), data.size(), &texSize.x, &texSize.y, reinterpret_cast<int*>(&components), static_cast<int>(settings.components));
                break;
            }
        }

        // Allocate and bind the texture.
        glGenTextures(1, &texture.textureId);
        glBindTexture(static_cast<GLenum>(settings.type), texture.textureId);

        // Handle mips
        if (settings.generateMipmaps)
        {
            glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(settings.min));
            glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(settings.mag));
        }

        // Handle wrapping behavior.
        glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_WRAP_R, static_cast<GLint>(settings.wrapR));
        glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_WRAP_S, static_cast<GLint>(settings.wrapS));
        glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_WRAP_T, static_cast<GLint>(settings.wrapT));

        // Construct the texture using the loaded data.
        glTexImage2D(
            static_cast<GLenum>(settings.type),
            0,							
            static_cast<GLint>(settings.intendedFormat),
            texSize.x,
            texSize.y,
            0,							
            components_to_format[static_cast<int>(settings.components)],
            channels_to_glenum[static_cast<uint>(settings.fileFormat)],
            imageData);

        // Generate mips.
        if (settings.generateMipmaps)
            glGenerateMipmap(static_cast<GLenum>(settings.type));

        // Cleanup and return.
        stbi_image_free(imageData);
        return decay(Ok(texture));
    }
}
