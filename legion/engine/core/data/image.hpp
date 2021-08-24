#pragma once
#include <core/platform/platform.hpp>
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

    struct image;

    namespace detail
    {
        void _destroy_impl(image& img);
    }

    /**@class image
     * @brief Object encapsulating the binary representation of an image.
     */
    struct image final
    {
        friend void detail::_destroy_impl(image& img);

        image(const math::ivec2& res, channel_format format, image_components comp, const data_view<byte>& data);

        MOVE_FUNCS_NOEXCEPT(image);
        COPY_FUNCS_NOEXCEPT(image);

        ~image();

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

        common::result<void> apply_raw(bool lazyApply = true);

        /**@brief Convert the binary image representation to a more usable representation if it hasn't been converted before and return the new representation.
         * @return const std::vector<math::color>& List with all the colors in the image.
         */
        common::result<std::reference_wrapper<const std::vector<math::color>>> read_colors() const;
        common::result<void> write_colors(const std::vector<math::color>& colors);

        /**@brief Get the data size of the binary data.
         */
        size_type data_size() const;

        bool operator==(const image& other) const noexcept;

        byte* data() noexcept;

    private:
        common::result<void> _apply_raw_impl() const;

        math::ivec2 m_resolution;
        channel_format m_format;
        image_components m_components;

        mutable data_view<byte> m_data;
        mutable std::optional<std::vector<math::color>> m_colors = std::nullopt;
    };

    namespace detail
    {
        void _destroy_impl(image& img)
        {
            img.m_data = data_view<byte>(nullptr);
        }
    }

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

            import_settings() noexcept :
                detectFormat(true),
                detectComponents(true),
                flipVertical(true),
                fileFormat(channel_format::eight_bit),
                components(image_components::rgba)
            {}

            NO_DEFAULT_CTOR_RULE5_NOEXCEPT(import_settings);
        };
    }
}

#include <core/data/image.inl>
