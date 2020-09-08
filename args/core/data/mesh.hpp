#pragma once
#include <core/types/primitives.hpp>
#include <core/math/math.hpp>
#include <core/filesystem/resource.hpp>
#include <vector>

namespace args::core
{
    struct sub_mesh
    {
        std::string name;
        size_type indexCount;
        size_type indexOffset;
    };

    struct ARGS_API mesh
    {
        std::string fileName;
        std::vector<math::vec3> vertices;
        std::vector<math::vec3> normals;
        std::vector<math::vec2> uvs;
        std::vector<math::vec3> tangents;
        std::vector<uint> indices;

        std::vector<sub_mesh> submeshes;

        static void to_resource(filesystem::basic_resource* resource, const mesh& value);
        static void from_resource(mesh* value, const filesystem::basic_resource& resource);
        static void calculate_tangents(mesh* data);
    };
}
