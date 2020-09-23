#include <rendering/data/material.hpp>

namespace args::rendering
{
    material_parameter_base* material_parameter_base::create_param(const std::pair<std::string, GLenum>& info)
    {
        switch (info.second)
        {
        case GL_FLOAT:
            return new material_parameter<float>(info.first);
            break;
        case GL_FLOAT_VEC2:
            return new material_parameter<math::vec2>(info.first);
            break;
        case GL_FLOAT_VEC3:
            return new material_parameter<math::vec3>(info.first);
            break;
        case GL_FLOAT_VEC4:
            return new material_parameter<math::vec4>(info.first);
            break;
        case GL_INT:
            return new material_parameter<int>(info.first);
            break;
        case GL_INT_VEC2:
            return new material_parameter<math::ivec2>(info.first);
            break;
        case GL_INT_VEC3:
            return new material_parameter<math::ivec3>(info.first);
            break;
        case GL_INT_VEC4:
            return new material_parameter<math::ivec4>(info.first);
            break;
        case GL_BOOL:
            return new material_parameter<bool>(info.first);
            break;
        case GL_BOOL_VEC2:
            return new material_parameter<math::bvec2>(info.first);
            break;
        case GL_BOOL_VEC3:
            return new material_parameter<math::bvec3>(info.first);
            break;
        case GL_BOOL_VEC4:
            return new material_parameter<math::bvec4>(info.first);
            break;
        case GL_FLOAT_MAT2:
            return new material_parameter<math::mat2>(info.first);
            break;
        case GL_FLOAT_MAT3:
            return new material_parameter<math::mat3>(info.first);
            break;
        case GL_FLOAT_MAT4:
            return new material_parameter<math::mat4>(info.first);
            break;
        default:
            return nullptr;
        }
    }

    async::readonly_rw_spinlock MaterialCache::m_materialLock;
    std::unordered_map<id_type, material> MaterialCache::m_materials;

    material_handle MaterialCache::create_material(const std::string& name, const shader_handle& shader)
    {
        id_type id = nameHash(name);
        if (m_materials.count(id))
            return { id };

        if (shader == invalid_shader_handle)
        {
            log::error("Tried to create a material named {} with an invalid shader.", name);
            return invalid_material_handle;
        }

        m_materials[id].init(shader);
        m_materials[id].m_name = name;

        log::debug("Created material {} with shader: {}", name, shader.get_name());

        return { id };
    }

    material_handle MaterialCache::create_material(const std::string& name, const filesystem::view& shaderFile, shader_import_settings settings)
    {
        id_type id = nameHash(name);
        if (m_materials.count(id))
            return { id };

        auto shader = ShaderCache::create_shader(shaderFile.get_filename(), shaderFile, settings);

        if (shader == invalid_shader_handle)
        {
            log::error("Tried to create a material named {} with an invalid shader file: {}.", name, shaderFile.get_path());
            return invalid_material_handle;
        }

        m_materials[id].init(shader);
        m_materials[id].m_name = name;

        log::debug("Created material {} with shader: {}", name, shader.get_name());

        return { id };
    }

    material_handle MaterialCache::get_material(const std::string& name)
    {
        id_type id = nameHash(name);
        async::readonly_guard guard(m_materialLock);
        if (m_materials.count(id))
            return { id };
        return invalid_material_handle;
    }

    void material_handle::bind()
    {
        async::readonly_guard guard(MaterialCache::m_materialLock);
        MaterialCache::m_materials[id].m_shader.bind();
    }

    void material_handle::prepare()
    {
        async::readonly_guard guard(MaterialCache::m_materialLock);
        for (auto& [_, param] : MaterialCache::m_materials[id].m_parameters)
            param->apply(MaterialCache::m_materials[id].m_shader);
    }

    attribute material_handle::get_attribute(const std::string& name)
    {
        return MaterialCache::m_materials[id].m_shader.get_attribute(name);
    }

}
