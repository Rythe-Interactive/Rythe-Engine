#pragma once
#include <core/core.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/mesh_splitter_utils/mesh_splitter.hpp>
#include <physics/data/fractureparams.hpp>
namespace legion::physics
{
    struct physics_manifold;

    struct FracturerColliderToMeshPairing
    {
        FracturerColliderToMeshPairing(
            std::shared_ptr<ConvexCollider> pColliderPair,
            ecs::component<MeshSplitter> pMeshSplitterPairing) :
            colliderPair(pColliderPair) , meshSplitterPairing(pMeshSplitterPairing)
        {
            assert(pMeshSplitterPairing.entity);
        }

        std::shared_ptr<ConvexCollider> colliderPair;
        ecs::component<MeshSplitter> meshSplitterPairing;

        void GenerateSplittingParamsFromCollider(std::shared_ptr<ConvexCollider> instantiatedCollider
            , std::vector<physics::MeshSplitParams>& meshSplitParams);
        

    };

    struct Fracturer
    {


        void HandleFracture(physics_manifold& manifold,bool& manifoldValid, bool isfracturingA);

        void ExplodeEntity(ecs::entity_handle ownerEntity,
            const FractureParams& fractureParams, PhysicsCollider* entityCollider = nullptr);;

        bool IsFractureConditionMet(physics_manifold& manifold, bool isfracturingA);

        void InitializeVoronoi(ecs::component<physicsComponent> physicsComponent);

        void InvestigateColliderToMeshPairing(ecs::entity_handle ent
            ,std::vector<FracturerColliderToMeshPairing> colliderToMeshPairings);

        void InstantiateColliderMeshPairingWithEntity(ecs::entity_handle ent,
            std::vector< FracturerColliderToMeshPairing>& colliderToMeshPairings);

        void GetVoronoiPoints(std::vector<std::vector<math::vec3>>& groupedPoints,
            std::vector<math::vec3>& voronoiPoints, math::vec3 min, math::vec3 max);

        void InstantiateVoronoiColliders(std::vector<std::shared_ptr<ConvexCollider>>& voronoiColliders
            , std::vector<std::vector<math::vec3>>& groupedPoints);

        void GenerateFractureFragments(std::vector<ecs::entity_handle>& entitiesGenerated
            , std::vector< FracturerColliderToMeshPairing>& colliderToMeshPairings
            , std::vector< std::shared_ptr<ConvexCollider>>& voronoiColliders
            , ecs::entity_handle fracturedEnt);

        void QuadrantVoronoi(math::vec3& min, math::vec3& max, std::vector<math::vec3>& voronoiPoints);

        void BalancedVoronoi(math::vec3& min, math::vec3& max, std::vector<math::vec3>& voronoiPoints);

        math::vec3 GetImpactPointFromManifold(physics_manifold& manifold);

        int fractureCount = 0;

        std::vector<std::vector<math::vec3>> verticesList;
        std::vector<std::shared_ptr<ConvexCollider>> debugVectorcolliders;
        std::vector<math::mat4> transforms;
        static ecs::EcsRegistry* registry;
    };

   

}

