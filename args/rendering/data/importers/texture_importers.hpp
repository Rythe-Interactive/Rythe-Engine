#pragma once
#include <rendering/data/texture.hpp>

namespace args::rendering
{
    struct ARGS_API stbi_texture_loader : public fs::resource_converter<texture, texture_import_settings>
    {
        constexpr static cstring extensions[] = { ".png", ".jpg", ".jpeg", ".jpe", ".jfif", ".jfi", ".jif", ".bmp", ".dib", ".raw", ".psd", ".psb", ".tga", ".icb", ".vda", ".vst", ".hdr", ".ppm", ".pgm" };

        virtual common::result_decay_more<fs::basic_resource, fs_error> load(const fs::basic_resource& resource, texture_import_settings&& settings) override;
    };
}
