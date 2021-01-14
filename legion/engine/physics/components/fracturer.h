#pragma once
#include <core/core.hpp>
#include <physics/components/physics_component.hpp>

namespace legion::physics
{
    struct physics_manifold;

    struct FracturerColliderToMeshPairing
    {
        FracturerColliderToMeshPairing(
            std::shared_ptr<ConvexCollider> pColliderPair,
            ecs::component_handle<mesh_filter> pMeshFilterPairing) :
            colliderPair(pColliderPair) , meshFilterPairing(pMeshFilterPairing)
        {

        }

        std::shared_ptr<ConvexCollider> colliderPair;
        ecs::component_handle<mesh_filter> meshFilterPairing;
    };

	struct Fracturer
	{


		void HandleFracture(physics_manifold& manifold,bool& manifoldValid, bool isfracturingA);

        bool IsFractureConditionMet();

        void InitializeVoronoi(ecs::component_handle<physicsComponent> physicsComponent);

        void InvestigateColliderToMeshPairing(ecs::entity_handle ent
            ,std::vector<FracturerColliderToMeshPairing> colliderToMeshPairings);

        int fractureCount = 0;

        std::vector<std::vector<math::vec3>> verticesList;
        std::vector<std::shared_ptr<ConvexCollider>> colliders;
        std::vector<math::mat4> transforms;
	};

   

}

