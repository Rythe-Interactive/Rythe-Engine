#pragma once
#include <core/core.hpp>

namespace legion::rendering
{
    enum shader_compiler_options
    {
        debug = 1,
        low_power = 2,
        api_opengl = 4,
        api_vulkan = 8
    };

    enum rendering_api
    {
        opengl = 4,
        vulkan = 8
    };

    constexpr bitfield8 default_shader_compiler_settings =  0 | shader_compiler_options::api_opengl;

    struct shader_import_settings
    {
        bool usePrecompiledIfAvailable;
        bool storePrecompiled;
        bitfield8 api;
        bool debug;
        bool low_power;
    };

    constexpr shader_import_settings default_shader_settings{ true, true, rendering_api::opengl, false, false };
}
