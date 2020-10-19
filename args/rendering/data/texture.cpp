#include <rendering/data/texture.hpp>

namespace args::rendering
{
    void texture::to_resource(fs::basic_resource* resource, const texture& value)
    {
        resource->clear();
        appendBinaryData(&value.textureId, resource->get());
        appendBinaryData(&value.size.x, resource->get());
        appendBinaryData(&value.size.y, resource->get());
        appendBinaryData(&value.channels, resource->get());
        appendBinaryData(&value.type, resource->get());
    }

    void texture::from_resource(texture* value, const fs::basic_resource& resource)
    {
        byte_vec::const_iterator start = resource.begin();
        retrieveBinaryData(value->textureId, start);
        retrieveBinaryData(value->size.x, start);
        retrieveBinaryData(value->size.y, start);
        retrieveBinaryData(value->channels, start);
        retrieveBinaryData(value->type, start);
    }

    sparse_map<id_type, texture> TextureCache::m_textures;
    async::readonly_rw_spinlock TextureCache::m_textureLock;

    texture_data texture_handle::get_data()
    {
        return TextureCache::get_data(id);
    }

    const texture& texture_handle::get_texture()
    {
        return TextureCache::get_texture(id);
    }

    const texture& TextureCache::get_texture(id_type id)
    {
        async::readonly_guard guard(m_textureLock);
        return m_textures[id];
    }

    texture_data TextureCache::get_data(id_type id)
    {
        texture texture;

        {
            async::readonly_guard guard(m_textureLock);
            texture = m_textures[id];
        }

        texture_data data{};
        data.size.x = texture.size.x;
        data.size.y = texture.size.y;
        data.type = texture.type;
        data.pixels.resize(data.size.x * data.size.y);
        glGetTexImage(static_cast<GLenum>(data.type), 0, components_to_format[static_cast<int>(texture.channels)], GL_RGBA, data.pixels.data());
        return data;
    }

    texture_handle TextureCache::create_texture(const std::string& name, const fs::view& file, texture_import_settings settings)
    {
        id_type id = nameHash(name);

        {
            async::readonly_guard guard(m_textureLock);
            if (m_textures.contains(id))
                return { id };
        }

        if (!file.is_valid() || !file.file_info().is_file)
            return invalid_texture_handle;

        auto result = fs::AssetImporter::tryLoad<texture>(file, settings);

        if (result != common::valid)
            return invalid_texture_handle;

        {
            async::readwrite_guard guard(m_textureLock);
            m_textures.insert(id, result);
        }

        return { id };
    }

    texture_handle TextureCache::create_texture(const std::string& name, texture_import_settings settings)
    {
        image_handle image = ImageCache::get_handle(name);
        if (image == invalid_image_handle)
        {
            log::warn("Image {} doesn't exist.", name);
            return invalid_texture_handle;
        }

        return create_texture(image, settings);
    }

    texture_handle TextureCache::create_texture(image_handle image, texture_import_settings settings)
    {
        if (image == invalid_image_handle)
        {
            log::warn("Tried to create a texture with an invalid image");
            return invalid_texture_handle;
        }

        id_type id = image.id;

        {
            async::readonly_guard guard(m_textureLock);
            if (m_textures.contains(id))
                return { id };
        }

        texture texture{};
        texture.type = settings.type;

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

        auto [lock, img] = image.get_raw_image();
        {
            async::readonly_guard guard(lock);

            texture.size = img.size;
            texture.channels = img.components;

            // Construct the texture using the loaded data.
            glTexImage2D(
                static_cast<GLenum>(settings.type),
                0,
                static_cast<GLint>(settings.intendedFormat),
                texture.size.x,
                texture.size.y,
                0,
                components_to_format[static_cast<int>(img.components)],
                channels_to_glenum[static_cast<uint>(img.format)],
                img.get_raw_data<void>());
        }

        // Generate mips.
        if (settings.generateMipmaps)
            glGenerateMipmap(static_cast<GLenum>(settings.type));

        {
            async::readwrite_guard guard(m_textureLock);
            m_textures.insert(id, texture);
        }

        return { id };
    }

    texture_handle TextureCache::get_handle(const std::string& name)
    {
        id_type id = nameHash(name);
        async::readonly_guard guard(m_textureLock);
        if (m_textures.contains(id))
            return { id };
        return invalid_texture_handle;
    }

    texture_handle TextureCache::get_handle(id_type id)
    {
        async::readonly_guard guard(m_textureLock);
        if (m_textures.contains(id))
            return { id };
        return invalid_texture_handle;
    }

}
