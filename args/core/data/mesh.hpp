#pragma once
#include <core/types/primitives.hpp>
#include <core/math/math.hpp>
#include <core/filesystem/resource.hpp>
#include <core/filesystem/view.hpp>
#include <core/async/readonly_rw_spinlock.hpp>

#include <utility>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

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

    struct ARGS_API mesh_handle
    {
        id_type id;

        std::pair<async::readonly_rw_spinlock&, mesh&> get();

        bool operator==(const mesh_handle& other) const { return id == other.id; }
    };

    constexpr mesh_handle invalid_mesh_handle{ invalid_id };

    struct mesh_import_settings
    {
        bool triangulate;
        bool vertex_color;
    };

    constexpr mesh_import_settings default_mesh_settings{ true, false };

    class ARGS_API MeshCache
    {
        friend struct mesh_handle;
    private:
        static std::unordered_map<id_type, std::unique_ptr<std::pair<async::readonly_rw_spinlock, mesh>>> m_meshes;
        static async::readonly_rw_spinlock m_meshesLock;

    public:
        static mesh_handle create_mesh(const std::string& name, const filesystem::view& file, mesh_import_settings settings = default_mesh_settings);
        static mesh_handle copy_mesh(const std::string& name, const std::string& newName);
        static mesh_handle copy_mesh(id_type id, const std::string& newName);
        static mesh_handle get_handle(const std::string& name);
        static mesh_handle get_handle(id_type id);
    };
}
