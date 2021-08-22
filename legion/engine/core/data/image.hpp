#pragma once
#include <core/types/types.hpp>
#include <core/containers/containers.hpp>
#include <core/filesystem/filesystem.hpp>
#include <core/assets/assets.hpp>
#include <core/math/math.hpp>

/**
 * @file image.hpp
 */

namespace legion::core
{
    /**@brief Internal binary data representation per color channel.
     */
    enum struct channel_format : uint
    {
        depth_stencil = 0,
        eight_bit = 1,
        sixteen_bit = 2,
        float_hdr = 4
    };

    /**@brief Internal channel layout of the colors.
     */
    enum struct image_components : int
    {
        grey = 1,
        grey_alpha = 2,
        rgb = 3,
        rgba = 4,
        depth = 5,
        stencil = 6,
        depth_stencil = 7
    };

    /**@class image
     * @brief Object encapsulating the binary representation of an image.
     */
    struct image final
    {
        image(math::ivec2 res, channel_format format, image_components comp, data_view<byte> data);

        const math::ivec2& resolution() const noexcept;
        const channel_format& format() const noexcept;
        const image_components& components() const noexcept;

        /**@brief Get the binary representation of the image with different pointer types.
         *        Each pointer type is only enabled if the channel format is the same.
         *        The void* type is always enabled.
         */
        template<typename T>
        data_view<T> raw_data();

        template<typename T>
        const data_view<T> raw_data() const;

        /**@brief Convert the binary image representation to a more usable representation if it hasn't been converted before and return the new representation.
         * @return const std::vector<math::color>& List with all the colors in the image.
         */
        const std::vector<math::color>& read_colors() const;
        void write_colors(const std::vector<math::color>& colors);

        /**@brief Get the data size of the binary data.
         */
        size_type data_size();

        bool operator==(const image& other)
        {
            return m_data == other.m_data;
        }

        byte* data() noexcept { return m_data.data(); }

    private:
        math::ivec2 m_resolution;
        channel_format m_format;
        image_components m_components;

        mutable data_view<byte> m_data = nullptr;
    };

    namespace assets
    {
        /**@class import_settings<image>
         * @brief Data structure to parameterize the image import process.
         */
        template<>
        struct import_settings<image>
        {
            bool detectFormat : 1;
            bool detectComponents : 1;
            bool flipVertical : 1;
            channel_format fileFormat;
            image_components components;
        };
    }


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

    /**@class image_handle
     * @brief Save to pass around handle to a raw image in the image cache.
     */
    struct image_handle
    {
        id_type id;

        math::ivec2 size();

        /**@brief Convert the binary image representation to a more usable representation if it hasn't been converted before and return the new representation.
         * @return const std::vector<math::color>& List with all the colors in the image.
         */
        const std::vector<math::color>& read_colors();

        /**@brief Get the image and the attached lock. Will return invalid_image if the handle was invalid.
         */
        std::pair<async::rw_spinlock&, image&> get_raw_image();

        void destroy();

        bool operator==(const image_handle& other) { return id == other.id; }
        operator id_type() { return id; }
    };

    /**@brief Default invalid image handle.
     */
    constexpr image_handle invalid_image_handle{ invalid_id };

    /**@brief Default image import settings.
     */
    constexpr image_import_settings default_image_settings{ channel_format::eight_bit, image_components::rgba, true };

    /**@class ImageCache
     * @brief Data cache for loading, storing and managing raw images.
     */
    class ImageCache
    {
        friend class renderer;
        friend struct image;
        friend struct image_handle;
    private:
        static const std::vector<math::color> m_nullColors;
        static async::rw_spinlock m_nullLock;

        static std::unordered_map<id_type, std::unique_ptr<std::pair<async::rw_spinlock, image>>> m_images;
        static async::rw_spinlock m_imagesLock;
        static std::unordered_map<id_type, std::unique_ptr<std::vector<math::color>>> m_colors;
        static async::rw_spinlock m_colorsLock;

        static const std::vector<math::color>& process_raw(id_type id);

        static const std::vector<math::color>& read_colors(id_type id);
        static std::pair<async::rw_spinlock&, image&> get_raw_image(id_type id);

    public:
        /**@brief Create a new image and load it from a file if a image with the same name doesn't exist yet.
         * @param name Identifying name for the image.
         * @param file File to load from.
         * @param settings Settings to pass on to the import pipeline.
         * @return image_handle A valid handle to the newly created image if it succeeds, invalid_image_handle if it fails.
         */
        static image_handle create_image(const std::string& name, const filesystem::view& file, image_import_settings settings = default_image_settings);
        static image_handle create_image(const filesystem::view& file, image_import_settings settings = default_image_settings);
        static image_handle insert_image(image&& img);

        /**@brief Returns a handle to a image with a certain name. Will return invalid_image_handle if the requested image doesn't exist.
         */
        static image_handle get_handle(const std::string& name);

        /**@brief Returns a handle to a image with a certain name. Will return invalid_image_handle if the requested image doesn't exist.
         * @param id Name hash
         */
        static image_handle get_handle(id_type id);

        static void destroy_image(const std::string& name);

        static void destroy_image(id_type id);
    };
}
