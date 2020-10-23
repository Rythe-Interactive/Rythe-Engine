#pragma once
#include <rendering/data/texture.hpp>

/**
 * @file texture_importers.hpp
 */

namespace legion::rendering
{
    /**@class stbi_texture_loader
     * @brief Resource converter for loading textures using stb_image
     */
    struct stbi_texture_loader : public fs::resource_converter<texture, texture_import_settings>
    {
        // All file extensions supported by stb_image
        constexpr static cstring extensions[] = { "", ".png", ".jpg", ".jpeg", ".jpe", ".jfif", ".jfi", ".jif", ".bmp", ".dib", ".raw", ".psd", ".psb", ".tga", ".icb", ".vda", ".vst", ".hdr", ".ppm", ".pgm" };

        virtual common::result_decay_more<texture, fs_error> load(const fs::basic_resource& resource, texture_import_settings&& settings) override;
    };
}
