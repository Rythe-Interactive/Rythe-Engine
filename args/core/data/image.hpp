#pragma once
#include <core/types/primitives.hpp>
#include <core/containers/sparse_map.hpp>
#include <core/math/color.hpp>
#include <core/async/readonly_rw_spinlock.hpp>
#include <core/filesystem/view.hpp>

namespace args::core
{
    enum struct channel_format : uint
    {
        eight_bit = 1,
        sixteen_bit = 2,
        float_hdr = 4
    };

    enum struct image_components : int
    {
        grey = 1,
        grey_alpha = 2,
        rgb = 3,
        rgba = 4
    };

    struct image
    {
        friend struct stb_image_loader;
        friend class ImageCache;

        math::ivec2 size;
        channel_format format;
        image_components components;

        template<typename T>
        T* get_raw_data();
        void apply_raw(bool lazyApply = true);

        size_type data_size();


        bool operator==(const image& other)
        {
            return m_id == other.m_id;
        }

    private:
        id_type m_id;
        size_type m_dataSize;
        byte* m_pixels;
    };

    inline static image invalid_image{};

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


    struct image_handle
    {
        id_type id;
        math::ivec2 size();
        const std::vector<math::color>& read_colors();
        std::pair<async::readonly_rw_spinlock&, image&> get_raw_image();

        bool operator==(const image_handle& other) { return id == other.id; }
    };

    constexpr image_handle invalid_image_handle{ invalid_id };


    struct image_import_settings
    {
        channel_format fileFormat;
        image_components components;
        bool flipVertical;
    };

    constexpr image_import_settings default_image_settings{ channel_format::eight_bit, image_components::rgba, true };

    class ImageCache
    {
        friend class renderer;
        friend struct image;
        friend struct image_handle;
    private:
        static std::vector<math::color> m_nullColors;
        static async::readonly_rw_spinlock m_nullLock;

        static std::unordered_map<id_type, std::unique_ptr<std::pair<async::readonly_rw_spinlock, image>>> m_images;
        static async::readonly_rw_spinlock m_imagesLock;
        static std::unordered_map<id_type, std::unique_ptr<std::vector<math::color>>> m_colors;
        static async::readonly_rw_spinlock m_colorsLock;

        static const std::vector<math::color>& process_raw(id_type id);

        static const std::vector<math::color>& read_colors(id_type id);
        static std::pair<async::readonly_rw_spinlock&, image&> get_raw_image(id_type id);
    public:
        static image_handle create_image(const std::string& name, const filesystem::view& file, image_import_settings settings = default_image_settings);
        static image_handle get_handle(const std::string& name);
        static image_handle get_handle(id_type id);
    };
}
