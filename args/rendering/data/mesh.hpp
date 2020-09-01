#pragma once
#include <rendering/detail/engine_include.hpp>

#include <vector>
#include <unordered_map>
#include <set>

namespace args::rendering
{
    struct attribute;

    struct submesh_data
    {
        std::string name;
        std::vector<uint> indices;
    };

    struct ARGS_API mesh_data
    {
        std::string fileName;
        std::vector<math::vec3> vertices;
        std::vector<math::vec3> normals;
        std::vector<math::vec2> uvs;
        std::vector<math::vec3> tangents;

        std::vector<submesh_data> submeshes;

        static void to_resource(fs::basic_resource* resource, const mesh_data& value);
        static void from_resource(mesh_data* value, const fs::basic_resource& resource);
        static void calculate_tangents(mesh_data* data);
    };

    struct sub_mesh
    {
        uint indexCount;
        app::gl_id indexBufferId{};
    };

    struct mesh
    {
        bool buffered;
        app::gl_id vertexBufferId;
        app::gl_id normalBufferId;
        app::gl_id uvBufferId;
        app::gl_id tangentBufferId;

        std::vector<sub_mesh> submeshes;
    };

    struct ARGS_API mesh_handle
    {
        id_type id;
        
        bool operator==(const mesh_handle& other) const { return id == other.id; }
        bool is_buffered();
        void buffer_data();
        const mesh_data& get_data();
        const mesh& get_mesh();
    };

    constexpr mesh_handle invalid_mesh_handle { 0 };

    struct mesh_import_settings
    {
        bool triangulate;
        bool vertex_color;
    };

    constexpr mesh_import_settings default_mesh_settings { true, false };

    struct ARGS_API mesh_cache
    {
        friend class renderer;
        friend struct mesh_handle;
    private:
        static sparse_map<id_type, mesh> m_meshes;
        static async::readonly_rw_spinlock m_meshLock;

        static sparse_map<id_type, std::unique_ptr<mesh_data>> m_meshdata;
        static async::readonly_rw_spinlock m_dataLock;

        static const mesh& get_mesh(id_type id);
        static const mesh_data& get_data(id_type id);
        
    public:
        static void buffer(id_type id);
        static mesh_handle create_mesh(const std::string& name, const fs::view& file, mesh_import_settings settings = default_mesh_settings);
        static mesh_handle get_handle(const std::string& name);
        static mesh_handle get_handle(id_type id);
    };
}

namespace std
{
    template<>
    struct hash<args::rendering::mesh_handle>
    {
        std::size_t operator()(args::rendering::mesh_handle const& handle) const noexcept
        {
            return static_cast<std::size_t>(handle.id);
        }
    };
}
