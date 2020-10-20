#pragma once
#include <core/types/primitives.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/math/color.hpp>
#include <core/async/readonly_rw_spinlock.hpp>
#include <core/filesystem/view.hpp>
#include <mutex>

/**
 * @file image.hpp
 */

namespace legion::core
{
    /**@brief Internal binary data representation per color channel.
     */
    enum struct channel_format : uint
    {
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
        rgba = 4
    };

    /**@class image
     * @brief Object encapsulating the binary representation of an image.
     */
    struct image final
    {
        friend struct stb_image_loader;
        friend class ImageCache;

        math::ivec2 size;
        channel_format format;
        image_components components;

        /**@brief Get the binary representation of the image with different pointer types.
         *        Each pointer type is only enabled if the channel format is the same.
         *        The void* type is always enabled.
         */
        template<typename T>
        T* get_raw_data();

        /**@brief Apply changes made to the binary data gotten with get_raw_data() to the colors read with read_colors().
         * @param lazyApply Apply immediately if false, otherwise the changes will only actually be applied when read_colors() is called.
         */
        void apply_raw(bool lazyApply = true);

        /**@brief Convert the binary image representation to a more usable representation if it hasn't been converted before and return the new representation.
         * @return const std::vector<math::color>& List with all the colors in the image.
         */
        const std::vector<math::color>& read_colors();

        /**@brief Get the data size of the binary data.
         */
        size_type data_size();

        bool operator==(const image& other)
        {
            return m_id == other.m_id;
        }

        image() = default;

        image(const image& other) : size(other.size), format(other.format), components(other.components), m_id(other.m_id), m_dataSize(other.m_dataSize), m_pixels(other.m_pixels)
        {
            if (m_id)
            {
                std::lock_guard guard(m_refsLock);
                m_refs[m_id]++;
            }
        }

        image(image&& other) : size(other.size), format(other.format), components(other.components), m_id(other.m_id), m_dataSize(other.m_dataSize), m_pixels(other.m_pixels)
        {
            if (m_id)
            {
                std::lock_guard guard(m_refsLock);
                m_refs[m_id]++;
            }
        }

        image& operator=(const image& other)
        {

            {
                std::lock_guard guard(m_refsLock);

                if (m_id)
                {
                    m_refs[m_id]--;
                    if (m_refs[m_id] == 0)
                    {
                        delete[] m_pixels;
                        m_pixels = nullptr;
                        m_refs.erase(m_id);
                    }
                }

                m_id = other.m_id;

                m_refs[m_id]++;
            }

            size = other.size;
            format = other.format;
            components = other.components;
            m_dataSize = other.m_dataSize;
            m_pixels = other.m_pixels;
            return *this;
        }

        image& operator=(image&& other)
        {
            {
                std::lock_guard guard(m_refsLock);

                if (m_id)
                {
                    m_refs[m_id]--;
                    if (m_refs[m_id] == 0)
                    {
                        delete[] m_pixels;
                        m_pixels = nullptr;
                        m_refs.erase(m_id);
                    }
                }

                m_id = other.m_id;

                m_refs[m_id]++;
            }

            size = other.size;
            format = other.format;
            components = other.components;
            m_dataSize = other.m_dataSize;
            m_pixels = other.m_pixels;
            return *this;
        }

        ~image()
        {
            if (m_id)
            {
                std::lock_guard guard(m_refsLock);

                m_refs[m_id]--;
                if (m_refs[m_id] == 0)
                {
                    delete[] m_pixels;
                    m_pixels = nullptr;
                    m_refs.erase(m_id);
                }
            }
        }

    private:

        static std::unordered_map<id_type, uint> m_refs;
        static std::mutex m_refsLock;

        id_type m_id;
        size_type m_dataSize;
        byte* m_pixels;
    };

    template<typename T>
    T* image::get_raw_data()
    {
        return nullptr;
    }

    template<>
    inline void* image::get_raw_data<void>()
    {
        return reinterpret_cast<void*>(m_pixels);
    }

    template<>
    inline byte* image::get_raw_data<byte>()
    {
        if (format == channel_format::eight_bit)
            return m_pixels;
        return nullptr;
    }

    template<>
    inline uint16* image::get_raw_data<uint16>()
    {
        if (format == channel_format::sixteen_bit)
            return reinterpret_cast<uint16*>(m_pixels);
        return nullptr;
    }

    template<>
    inline float* image::get_raw_data<float>()
    {
        if (format == channel_format::float_hdr)
            return reinterpret_cast<float*>(m_pixels);
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
        std::pair<async::readonly_rw_spinlock&, image&> get_raw_image();

        void destroy();

        bool operator==(const image_handle& other) { return id == other.id; }
        operator id_type() { return id; }
    };

    /**@brief Default invalid image handle.
     */
    constexpr image_handle invalid_image_handle{ invalid_id };

    /**@class image_import_settings
     * @brief Data structure to parameterize the image import process.
     */
    struct image_import_settings
    {
        channel_format fileFormat;
        image_components components;
        bool flipVertical;
    };

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
        static async::readonly_rw_spinlock m_nullLock;

        static std::unordered_map<id_type, std::unique_ptr<std::pair<async::readonly_rw_spinlock, image>>> m_images;
        static async::readonly_rw_spinlock m_imagesLock;
        static std::unordered_map<id_type, std::unique_ptr<std::vector<math::color>>> m_colors;
        static async::readonly_rw_spinlock m_colorsLock;

        static const std::vector<math::color>& process_raw(id_type id);

        static const std::vector<math::color>& read_colors(id_type id);
        static std::pair<async::readonly_rw_spinlock&, image&> get_raw_image(id_type id);

    public:
        /**@brief Create a new image and load it from a file if a image with the same name doesn't exist yet.
         * @param name Identifying name for the image.
         * @param file File to load from.
         * @param settings Settings to pass on to the import pipeline.
         * @return image_handle A valid handle to the newly created image if it succeeds, invalid_image_handle if it fails.
         */
        static image_handle create_image(const std::string& name, const filesystem::view& file, image_import_settings settings = default_image_settings);

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
