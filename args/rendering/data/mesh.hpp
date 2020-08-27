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
        app::gl_id vertexBufferId{};
        app::gl_id normalBufferId{};
        app::gl_id uvBufferId{};
        app::gl_id tangentBufferId{};

        std::vector<sub_mesh> submeshes;
    };

    struct ARGS_API mesh_handle
    {
        id_type id;

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
        static sparse_map<id_type, mesh> m_meshes;
        static async::readonly_rw_spinlock m_meshLock;

        static sparse_map<id_type, std::unique_ptr<mesh_data>> m_meshdata;
        static async::readonly_rw_spinlock m_dataLock;

        static const mesh& get_mesh(id_type id);

        static void buffer(mesh_data* data, mesh& mesh);
        
    public:
        static mesh_handle create_mesh(const std::string& name, const fs::view& file, mesh_import_settings settings = default_mesh_settings);
        static mesh_handle get_handle(const std::string& name);
        static mesh_handle get_handle(id_type id);
    };

    void mesh_data::to_resource(fs::basic_resource* resource, const mesh_data& value)
    {
        byte_vec data;

        appendBinaryData(&value.fileName, data);
        appendBinaryData(&value.vertices, data);
        appendBinaryData(&value.normals, data);
        appendBinaryData(&value.uvs, data);
        appendBinaryData(&value.tangents, data);

        uint64 submeshCount = value.submeshes.size();

        appendBinaryData(&submeshCount, data);
        
        for (auto& submesh : value.submeshes)
        {
            appendBinaryData(&submesh.name, data);
            appendBinaryData(&submesh.indices, data);
        }

        *resource = fs::basic_resource(data);
    }

    void mesh_data::from_resource(mesh_data* value, const fs::basic_resource& resource)
    {
        *value = mesh_data{};

        byte_vec::const_iterator start = resource.begin();
        
        retrieveBinaryData(value->fileName, start);
        retrieveBinaryData(value->vertices, start);
        retrieveBinaryData(value->normals, start);
        retrieveBinaryData(value->uvs, start);
        retrieveBinaryData(value->tangents, start);

        uint64 submeshCount;

        retrieveBinaryData(submeshCount, start);

        for (int i = 0; i <submeshCount; i++)
        {
            submesh_data submesh;
            retrieveBinaryData(submesh.name, start);
            retrieveBinaryData(submesh.indices, start);
            value->submeshes.push_back(submesh);
        }
    }

    void mesh_data::calculate_tangents(mesh_data* data)
    {
        for (unsigned i = 0; i < data->normals.size(); i++)
            data->tangents.push_back(math::vec3(0));

        for (auto& submesh : data->submeshes)
            for (unsigned i = 0; i < submesh.indices.size(); i += 3)
            {
                math::vec3 vtx0 = data->vertices[submesh.indices[i]];
                math::vec3 vtx1 = data->vertices[submesh.indices[i + 1]];
                math::vec3 vtx2 = data->vertices[submesh.indices[i + 2]];

                math::vec2 uv0 = data->uvs[submesh.indices[i]];
                math::vec2 uv2 = data->uvs[submesh.indices[i + 2]];
                math::vec2 uv1 = data->uvs[submesh.indices[i + 1]];

                math::vec3 edge0 = vtx1 - vtx0;
                math::vec3 edge1 = vtx2 - vtx0;

                math::vec2 deltaUV0 = uv1 - uv0;
                math::vec2 deltaUV1 = uv2 - uv0;

                float uvDetFrac = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

                math::vec3 tangent;
                tangent.x = uvDetFrac * (deltaUV1.y * edge0.x - deltaUV0.y * edge1.x);
                tangent.y = uvDetFrac * (deltaUV1.y * edge0.y - deltaUV0.y * edge1.y);
                tangent.z = uvDetFrac * (deltaUV1.y * edge0.z - deltaUV0.y * edge1.z);
                tangent = math::normalize(tangent);

                data->tangents[submesh.indices[i]] += tangent;
                data->tangents[submesh.indices[i + 1]] += tangent;
                data->tangents[submesh.indices[i + 2]] += tangent;
            }

        for (unsigned i = 0; i < data->tangents.size(); i++)
            data->tangents[i] = math::normalize(data->tangents[i]);
    }
}
