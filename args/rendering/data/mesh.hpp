#pragma once

#include <vector>
#include <unordered_map>
#include <set>
#include <application/application.hpp>

namespace args::rendering
{
    struct attribute;

    struct submesh_data
    {
        std::string name;
        std::vector<uint> indices;
    };

    struct mesh_data
    {
        std::string fileName;
        std::vector<math::vec3> vertices;
        std::vector<math::vec3> normals;
        std::vector<math::vec2> uvs;
        std::vector<math::vec3> tangents;

        std::vector<submesh_data> submeshes;
    };

    struct sub_mesh
    {
        uint indexCount;
        app::gl_id indexBufferId{};
    };

    struct mesh
    {
        app::gl_id vertexBufferId{};
        app::gl_id normalBufferId{};
        app::gl_id uvBufferId{};
        app::gl_id tangentBufferId{};

        std::vector<sub_mesh> submeshes;
    };

    struct ARGS_API mesh_handle
    {
        id_type id;

        void bind(attribute* verticesAttrib, attribute* normalsAttrib = nullptr, attribute* uvAttrib = nullptr, attribute* tangentsAttrib = nullptr) const;
        void draw(unsigned count) const;
        static void unbind(attribute* verticesAttrib, attribute* normalsAttrib = nullptr, attribute* uvAttrib = nullptr, attribute* tangentsAttrib = nullptr);
        mesh_data* get_data();
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
        static atomic_sparse_map<id_type, mesh> m_meshes;
        static sparse_map<id_type, std::unique_ptr<mesh_data>> m_meshdata;
        static async::readonly_rw_spinlock m_dataLock;

        static mesh get_mesh(id_type id);

        static void buffer(mesh_data* data, mesh& mesh);
        static void calculate_tangents(mesh_data* data);
        
    public:
        /**
         * Loads a mesh from an .obj file. The file has to have:
         * vertexes, uvs, normals and face indexes. See load source
         * for more format information.
         */
        static mesh_handle create_mesh(const std::string& name, const fs::view& file, mesh_import_settings settings = default_mesh_settings);
        static mesh_handle get_handle(const std::string& name);
        static mesh_handle get_handle(id_type id);
    };
}
