#include <physics/mesh_splitter_utils/primitive_mesh.h>
#include <rendering/components/renderable.hpp>

namespace legion::physics
{

    ecs::entity_handle PrimitiveMesh::InstantiateNewGameObject()
    {
        auto ent = m_ecs->createEntity();

        std::vector<math::vec3> vertices;
        std::vector<math::vec2> uvs;
        std::vector<uint> indices;

        vertices.push_back(math::vec3(0, 0, 0));
        vertices.push_back(math::vec3(0, 1, 0));
        vertices.push_back(math::vec3(0, 0, 1));

        uvs.push_back(math::vec3(0));
        uvs.push_back(math::vec3(0));
        uvs.push_back(math::vec3(0));

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        mesh newMesh;
        newMesh.vertices = vertices;
        newMesh.uvs = uvs;
        newMesh.indices = indices;

        newMesh.calculate_tangents(&newMesh);

        mesh_handle meshH = core::MeshCache::create_mesh("newMesh", newMesh);

        auto renderableHandle = m_ecs->createComponent<rendering::renderable>(ent);
       //rendering::ModelCache::create_model(meshH)

        auto transformH = m_ecs->createComponents<transform>(ent);
       

        return ent;
    }

    void PrimitiveMesh::SetECSRegistry(ecs::EcsRegistry* ecs)
    {
        m_ecs = ecs;
    }
}
