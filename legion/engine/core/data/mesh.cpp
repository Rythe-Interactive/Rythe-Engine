#include <core/data/mesh.hpp>

namespace legion::core
{
    void mesh::calculate_tangents(mesh* data)
    {
        OPTICK_EVENT();
        // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
        data->tangents.resize(data->normals.size());

        // Iterate over all triangles of each sub-mesh.
        for (auto& submesh : data->submeshes)
            for (size_type i = submesh.indexOffset; i < submesh.indexOffset + submesh.indexCount; i += 3)
            {
                if (i + 2 > data->indices.size())
                    break;

                math::uvec3 indices{ data->indices[i], data->indices[i + 1], data->indices[i + 2] };

                if (indices[0] > data->vertices.size() || indices[1] > data->vertices.size() || indices[2] > data->vertices.size())
                    continue;

                // Get vertices of the triangle.
                math::vec3 vtx0 = data->vertices[indices[0]];
                math::vec3 vtx1 = data->vertices[indices[1]];
                math::vec3 vtx2 = data->vertices[indices[2]];

                // Get UVs of the triangle.
                math::vec2 uv0 = data->uvs[indices[0]];
                math::vec2 uv1 = data->uvs[indices[1]];
                math::vec2 uv2 = data->uvs[indices[2]];

                // Get primary edges
                math::vec3 edge0 = vtx1 - vtx0;
                math::vec3 edge1 = vtx2 - vtx0;

                // Get difference in uv over the two primary edges.
                math::vec2 deltaUV0 = uv1 - uv0;
                math::vec2 deltaUV1 = uv2 - uv0;

                // Get inverse of the determinant of the UV tangent matrix.
                float inverseUVDeterminant = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

                // T = tangent
                // B = bi-tangent
                // E0 = first primary edge
                // E1 = second primary edge
                // dU0 = delta of x texture coordinates of the first primary edge
                // dV0 = delta of y texture coordinates of the first primary edge
                // dU1 = delta of x texture coordinates of the second primary edge
                // dV1 = delta of y texture coordinates of the second primary edge
                // ┌          ┐          1        ┌           ┐ ┌             ┐
                // │ Tx Ty Tz │ _ ─────────────── │  dV1 -dV0 │ │ E0x E0y E0z │
                // │ Bx By Bz │ ─ dU0ΔV1 - dU1ΔV0 │ -dU1  dU0 │ │ E1x E1y E1z │
                // └          ┘                   └           ┘ └             ┘
                math::vec3 tangent;
                tangent.x = inverseUVDeterminant * ((deltaUV1.y * edge0.x) - (deltaUV0.y * edge1.x));
                tangent.y = inverseUVDeterminant * ((deltaUV1.y * edge0.y) - (deltaUV0.y * edge1.y));
                tangent.z = inverseUVDeterminant * ((deltaUV1.y * edge0.z) - (deltaUV0.y * edge1.z));

                // Check if the tangent is valid.
                if (tangent == math::vec3(0, 0, 0) || tangent != tangent)
                    continue;

                // Normalize the tangent.
                tangent = math::normalize(tangent);

                // Accumulate the tangent in order to be able to smooth it later.
                data->tangents[indices[0]] += tangent;
                data->tangents[indices[1]] += tangent;
                data->tangents[indices[2]] += tangent;
            }

        // Smooth all tangents.
        for (size_type i = 0; i < data->tangents.size(); i++)
            if (data->tangents[i] != math::vec3::zero)
                data->tangents[i] = math::normalize(data->tangents[i]);
    }
}
