#include <core/data/mesh.hpp>

namespace args::core
{
    void mesh::to_resource(filesystem::basic_resource* resource, const mesh& value)
    {
        resource->clear();
        appendBinaryData(&value.fileName, resource->get());
        appendBinaryData(&value.vertices, resource->get());
        appendBinaryData(&value.normals, resource->get());
        appendBinaryData(&value.uvs, resource->get());
        appendBinaryData(&value.tangents, resource->get());
        appendBinaryData(&value.indices, resource->get());

        uint64 submeshCount = value.submeshes.size();
        appendBinaryData(&submeshCount, resource->get());

        for (auto& submesh : value.submeshes)
        {
            appendBinaryData(&submesh.name, resource->get());
            appendBinaryData(&submesh.indexCount, resource->get());
            appendBinaryData(&submesh.indexOffset, resource->get());
        }
    }

    void mesh::from_resource(mesh* value, const filesystem::basic_resource& resource)
    {
        *value = mesh{};

        byte_vec::const_iterator start = resource.begin();

        retrieveBinaryData(value->fileName, start);
        retrieveBinaryData(value->vertices, start);
        retrieveBinaryData(value->normals, start);
        retrieveBinaryData(value->uvs, start);
        retrieveBinaryData(value->tangents, start);
        retrieveBinaryData(value->indices, start);

        uint64 submeshCount;

        retrieveBinaryData(submeshCount, start);

        for (int i = 0; i < submeshCount; i++)
        {
            sub_mesh submesh;
            retrieveBinaryData(submesh.name, start);
            retrieveBinaryData(submesh.indexCount, start);
            retrieveBinaryData(submesh.indexOffset, start);
            value->submeshes.push_back(submesh);
        }
    }

    void mesh::calculate_tangents(mesh* data)
    {
        data->tangents.resize(data->normals.size());

        for (auto& submesh : data->submeshes)
            for (unsigned i = submesh.indexOffset; i < submesh.indexOffset + submesh.indexCount; i += 3)
            {
                math::vec3 vtx0 = data->vertices[data->indices[i]];
                math::vec3 vtx1 = data->vertices[data->indices[i + 1]];
                math::vec3 vtx2 = data->vertices[data->indices[i + 2]];

                math::vec2 uv0 = data->uvs[data->indices[i]];
                math::vec2 uv2 = data->uvs[data->indices[i + 2]];
                math::vec2 uv1 = data->uvs[data->indices[i + 1]];

                math::vec3 edge0 = vtx1 - vtx0;
                math::vec3 edge1 = vtx2 - vtx0;

                math::vec2 deltaUV0 = uv1 - uv0;
                math::vec2 deltaUV1 = uv2 - uv0;

                float uvDetFrac = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

                math::vec3 tangent;
                tangent.x = uvDetFrac * ((deltaUV1.y * edge0.x) - (deltaUV0.y * edge1.x));
                tangent.y = uvDetFrac * ((deltaUV1.y * edge0.y) - (deltaUV0.y * edge1.y));
                tangent.z = uvDetFrac * ((deltaUV1.y * edge0.z) - (deltaUV0.y * edge1.z));
                if (tangent == math::vec3(0, 0, 0) || tangent != tangent)
                    continue;

                tangent = math::normalize(tangent);

                data->tangents[data->indices[i]] += tangent;
                data->tangents[data->indices[i + 1]] += tangent;
                data->tangents[data->indices[i + 2]] += tangent;
            }

        for (unsigned i = 0; i < data->tangents.size(); i++)
            if (data->tangents[i] != math::vec3(0, 0, 0))
                data->tangents[i] = math::normalize(data->tangents[i]);
    }
}
