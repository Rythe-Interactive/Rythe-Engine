#define STBI_MAX_DIMENSIONS 16384
#define STBI_NO_GIF
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <rendering/data/importers/texture_importers.hpp>

namespace args::rendering
{
    common::result_decay_more<fs::basic_resource, fs_error> stbi_texture_loader::load(const fs::basic_resource& resource, texture_import_settings&& settings)
    {
        using common::Err, common::Ok;
        // decay overloads the operator of ok_type and operator== for valid_t.
        using decay = common::result_decay_more<fs::basic_resource, fs_error>;

        byte_vec data = resource.get();

        stbi_set_flip_vertically_on_load(settings.flipVertical);

        texture texture{};
        texture.channels = settings.components;
        texture.type = settings.type;

        texture_components components = texture_components::grey;

        void* imageData;

        switch (settings.fileFormat)
        {
            default: [[fallthrough]];
            case texture_channel_format::eight_bit:
            {
                imageData = stbi_load_from_memory(data.data(), data.size(), &texture.width, &texture.height, reinterpret_cast<int*>(&components), static_cast<int>(settings.components));
                break;
            }
            case texture_channel_format::sixteen_bit:
            {
                imageData = stbi_load_16_from_memory(data.data(), data.size(), &texture.width, &texture.height, reinterpret_cast<int*>(&components), static_cast<int>(settings.components));
                break;
            }
            case texture_channel_format::float_hdr:
            {
                imageData = stbi_loadf_from_memory(data.data(), data.size(), &texture.width, &texture.height, reinterpret_cast<int*>(&components), static_cast<int>(settings.components));
                break;
            }
        }

        glGenTextures(1, &texture.textureId);

        glBindTexture(static_cast<GLenum>(settings.type), texture.textureId);

        if (settings.generateMipmaps)
        {
            glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(settings.min));
            glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(settings.mag));
        }

        glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_WRAP_R, static_cast<GLint>(settings.wrapR));
        glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_WRAP_S, static_cast<GLint>(settings.wrapS));
        glTexParameteri(static_cast<GLenum>(settings.type), GL_TEXTURE_WRAP_T, static_cast<GLint>(settings.wrapT));

        glTexImage2D(
            static_cast<GLenum>(settings.type),
            0,							
            static_cast<GLint>(settings.intendedFormat),
            texture.width,
            texture.height,
            0,							
            components_to_format[static_cast<int>(settings.components)],
            static_cast<GLenum>(settings.fileFormat),
            imageData);

        if (settings.generateMipmaps)
            glGenerateMipmap(static_cast<GLenum>(settings.type));

        stbi_image_free(imageData);

        data.clear();

        appendBinaryData(&texture.textureId, data);
        appendBinaryData(&texture.width, data);
        appendBinaryData(&texture.height, data);
        appendBinaryData(&texture.channels, data);
        appendBinaryData(&texture.type, data);

        return decay(Ok(fs::basic_resource(data)));
    }
}
