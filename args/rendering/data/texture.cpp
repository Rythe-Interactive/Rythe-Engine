#include <rendering/data/texture.hpp>

namespace args::rendering
{
    void texture::to_resource(fs::basic_resource* resource, const texture& value)
    {
        resource->clear();
        appendBinaryData(&value.textureId, resource->get());
        appendBinaryData(&value.width, resource->get());
        appendBinaryData(&value.height, resource->get());
        appendBinaryData(&value.channels, resource->get());
        appendBinaryData(&value.type, resource->get());
    }

    void texture::from_resource(texture* value, const fs::basic_resource& resource)
    {
        byte_vec::const_iterator start = resource.begin();
        retrieveBinaryData(value->textureId, start);
        retrieveBinaryData(value->width, start);
        retrieveBinaryData(value->height, start);
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
        data.width = texture.width;
        data.height = texture.height;
        data.channels = texture.channels;
        data.type = texture.type;
        data.pixels.resize(data.width * data.height);
        glGetTexImage(static_cast<GLenum>(data.type), 0, components_to_format[static_cast<int>(data.channels)], GL_RGBA, data.pixels.data());
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
