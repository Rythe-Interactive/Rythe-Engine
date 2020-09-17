#pragma once
#include <rendering/detail/engine_include.hpp>

namespace args::rendering
{
    enum struct texture_type : GLenum
    {
        one_dimensional = GL_TEXTURE_1D,
        two_dimensional = GL_TEXTURE_2D,
        three_dimensional = GL_TEXTURE_3D,
        array_1D = GL_TEXTURE_1D_ARRAY,
        array_2D = GL_TEXTURE_2D_ARRAY,
        rectangle = GL_TEXTURE_RECTANGLE,
        cube_map_positive_x = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        cube_map_negative_x = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        cube_map_positive_y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        cube_map_negative_y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        cube_map_positive_z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        cube_map_negative_z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    enum struct texture_format : GLint
    {
        depth = GL_DEPTH_COMPONENT,
        stencil = GL_STENCIL_INDEX,
        depth_stencil = GL_DEPTH_STENCIL,
        red = GL_RED,
        rg = GL_RG,
        rgb = GL_RGB,
        rgba = GL_RGBA,
        bgr = GL_BGR,
        bgra = GL_BGRA,
        red_int = GL_RED_INTEGER,
        rg_int = GL_RG_INTEGER,
        rgb_int = GL_RGB_INTEGER,
        rgba_int = GL_RGBA_INTEGER,
        bgr_int = GL_BGR_INTEGER,
        bgra_int = GL_BGRA_INTEGER,
    };

    enum struct texture_channel_format : GLenum
    {
        eight_bit = GL_UNSIGNED_BYTE,
        sixteen_bit = GL_UNSIGNED_SHORT,
        float_hdr = GL_FLOAT
    };

    enum struct texture_components : int
    {
        grey = 1,
        gray_alpha = 2,
        rgb = 3,
        rgba = 4
    };

    constexpr GLenum components_to_format[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };

    enum struct texture_mipmap : GLint
    {
        nearest = GL_NEAREST,
        linear = GL_LINEAR
    };

    enum struct texture_wrap : GLint
    {
        edge_clamp = GL_CLAMP_TO_EDGE,
        border_clamp = GL_CLAMP_TO_BORDER,
        mirror = GL_MIRRORED_REPEAT,
        repeat = GL_REPEAT,
        mirror_then_clamp = GL_MIRROR_CLAMP_TO_EDGE
    };

    struct color : public math::vec4 { };

    struct texture_data
    {
        int width;
        int height;
        texture_components channels;
        texture_type type;

        std::vector<color> pixels;
    };

    struct texture
    {
        app::gl_id textureId = invalid_id;

        int width;
        int height;
        texture_components channels;
        texture_type type;

        static void to_resource(fs::basic_resource* resource, const texture& value);
        static void from_resource(texture* value, const fs::basic_resource& resource);
    };

    struct ARGS_API texture_handle
    {
        id_type id;

        texture_data get_data();
        const texture& get_texture();
    };

    constexpr texture_handle invalid_texture_handle { invalid_id };

    struct texture_import_settings
    {
        texture_type type;
        texture_channel_format fileFormat;
        texture_format intendedFormat;
        texture_components components;
        bool flipVertical;
        bool generateMipmaps;
        texture_mipmap min;
        texture_mipmap mag;
        texture_wrap wrapR;
        texture_wrap wrapS;
        texture_wrap wrapT;
    };

    constexpr texture_import_settings default_texture_settings{
        texture_type::two_dimensional, texture_channel_format::sixteen_bit, texture_format::rgba,
        texture_components::rgba, true, true, texture_mipmap::linear, texture_mipmap::linear,
        texture_wrap::repeat, texture_wrap::repeat, texture_wrap::repeat };

    class ARGS_API TextureCache
    {
        friend class renderer;
        friend struct texture_handle;
    private:
        static sparse_map<id_type, texture> m_textures;
        static async::readonly_rw_spinlock m_textureLock;

        static const texture& get_texture(id_type id);
        static texture_data get_data(id_type id);

    public:
        static texture_handle create_texture(const std::string& name, const fs::view& file, texture_import_settings settings = default_texture_settings);
        static texture_handle get_handle(const std::string& name);
        static texture_handle get_handle(id_type id);
    };
}
