#include <core/data/image.hpp>
#include <core/filesystem/assetimporter.hpp>

namespace args::core
{
    void image::apply_raw()
    {
        async::readwrite_guard guard(ImageCache::m_colorsLock);
        ImageCache::m_colors.erase(m_id);
    }

    const std::vector<math::color>& image_handle::read_colors()
    {
        return ImageCache::read_colors(id);
    }

    std::pair<async::readonly_rw_spinlock&, image&> image_handle::get_raw_image()
    {
        return ImageCache::get_raw_image(id);
    }

    const std::vector<math::color>& ImageCache::process_raw(id_type id)
    {
        // TODO
        return m_nullColors;
    }

    const std::vector<math::color>& ImageCache::read_colors(id_type id)
    {
        async::readonly_guard guard(m_colorsLock);
        if (!m_colors.count(id))
            return process_raw(id);
        return *m_colors[id];
    }

    std::pair<async::readonly_rw_spinlock&, image&> ImageCache::get_raw_image(id_type id)
    {
        async::readonly_guard guard(m_colorsLock);
        if (!m_images.count(id))
            return std::make_pair(std::ref(m_nullLock), std::ref(invalid_image));
        auto& [lock, image] = *m_images[id];
        return std::make_pair(std::ref(lock), std::ref(image));
    }

    image_handle ImageCache::create_image(const std::string& name, const filesystem::view& file, image_import_settings settings)
    {
        id_type id = nameHash(name);

        {
            async::readonly_guard guard(m_imagesLock);
            if (m_images.count(id))
                return { id };
        }

        if (!file.is_valid() || !file.file_info().is_file)
            return invalid_image_handle;

        auto result = filesystem::AssetImporter::tryLoad<image>(file, settings);

        if (result != common::valid)
            return invalid_image_handle;

        {
            async::readwrite_guard guard(m_imagesLock);
            auto* pair_ptr = new std::pair<async::readonly_rw_spinlock, image>(std::make_pair<async::readonly_rw_spinlock, image>(async::readonly_rw_spinlock(), std::move(result)));
            m_images.emplace(std::make_pair(id, std::unique_ptr<std::pair<async::readonly_rw_spinlock, image>>(pair_ptr)));
        }

        return { id };
    }

    image_handle ImageCache::get_handle(const std::string& name)
    {
        id_type id = nameHash(name);
        async::readonly_guard guard(m_imagesLock);
        if (m_images.count(id))
            return { id };
        return invalid_image_handle;
    }

    image_handle ImageCache::get_handle(id_type id)
    {
        async::readonly_guard guard(m_imagesLock);
        if (m_images.count(id))
            return { id };
        return invalid_image_handle;
    }

}
