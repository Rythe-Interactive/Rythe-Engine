#include <physics/components/fracturer.hpp>
#include <physics/data/physics_manifold.hpp>
#include <physics/physics_statics.hpp>
#include <physics/colliders/convexcollider.hpp>
#include <physics/data/identifier.hpp>
#include <physics/physics_statics.hpp>

namespace legion::physics
{
    ecs::EcsRegistry* Fracturer::registry = nullptr;

    void Fracturer::HandleFracture(physics_manifold& manifold, bool& manifoldValid,bool isfracturingA)
    {
        return;
        OPTICK_EVENT();
        if (!IsFractureConditionMet(manifold,isfracturingA) || manifold.contacts.empty()) { return; }
        
        //log::debug("manifold invalidated");
        manifoldValid = false;

        math::vec3 impactPoint = GetImpactPointFromManifold(manifold);
        float impactRadius = 0.2f;
        
        auto fracturedEnt = isfracturingA ? manifold.entityA : manifold.entityB;
        auto collider = isfracturingA ? manifold.colliderA : manifold.colliderB;

        FractureParams params(impactPoint, 0.0f);
        ExplodeEntity(fracturedEnt, params, collider);
    }

    void Fracturer::ExplodeEntity(ecs::entity_handle ownerEntity, const FractureParams& fractureParams, PhysicsCollider* entityCollider)
    {
        log::debug("------------------------------------- ExplodeEntity ---------------------------------------");

        if (!entityCollider)
        {
            log::debug("colliders size {} "
                , ownerEntity.read_component<physicsComponent>().colliders.size());

            auto physicsComp = ownerEntity.get_component_handle<physicsComponent>().read();
            entityCollider = physicsComp.colliders.at(0).get();

        }
        
        //ownerEntity.read_component<physicsComponent>().colliders.at(0)
        auto [min, max] = entityCollider->GetMinMaxWorldAABB();

        //-----------------------------------------------------------------------------------------------------------------------------//
                                //Generate a Voronoi Diagram, for now, the points are manually generated //
        //-----------------------------------------------------------------------------------------------------------------------------//

        std::vector<math::vec3> voronoiPoints;

        QuadrantVoronoi(min, max, voronoiPoints);

        //math::vec3 six = third + math::vec3(-0.2f, 0.0f, -0.0f);
        //voronoiPoints.push_back(six);

        for (auto point : voronoiPoints)
        {/*
            debug::drawLine(point,
                point + math::vec3(0, 0.1f, 0), math::colors::magenta, 8.0f, FLT_MAX, true);*/
        }

        std::vector<std::vector<math::vec3>> groupedPoints(voronoiPoints.size());

        GetVoronoiPoints(groupedPoints,
            voronoiPoints, min, max);

        //-----------------------------------------------------------------------------------------------------------------------------//
                                //Using the voronoi points, generate a set of colliders  //
        //-----------------------------------------------------------------------------------------------------------------------------//

        std::vector<std::shared_ptr<ConvexCollider>> voronoiColliders;

        InstantiateVoronoiColliders(voronoiColliders, groupedPoints);

        std::vector< FracturerColliderToMeshPairing> colliderToMeshPairings;

        //-----------------------------------------------------------------------------------------------------------------------------//
                                //From the mesh about to be fractured, get the pairs of colliders to Mesh  //
        //-----------------------------------------------------------------------------------------------------------------------------//

        InstantiateColliderMeshPairingWithEntity(ownerEntity,
            colliderToMeshPairings);

        for (size_t i = 0; i < ownerEntity.child_count(); i++)
        {
            auto child = ownerEntity.get_child(i);

            InstantiateColliderMeshPairingWithEntity(child,
                colliderToMeshPairings);
        }

        std::vector<ecs::entity_handle> entitiesGenerated;

        //-----------------------------------------------------------------------------------------------------------------------------//
                                //From the mesh about to be fractured, get the pairs of colliders to Mesh  //
        //-----------------------------------------------------------------------------------------------------------------------------//

        GenerateFractureFragments(entitiesGenerated, colliderToMeshPairings,
            voronoiColliders, ownerEntity);


        auto originalRB = ownerEntity.get_component_handle<rigidbody>().read();
        float fragmentMass = 1.0f / originalRB.inverseMass;
        fragmentMass /= entitiesGenerated.size();

        int colliderIter = 0;
        for (auto ent : entitiesGenerated)
        {
            auto [posH, rotH, scaleH] = ent.get_component_handles<transform>();
            math::mat4 trans = math::compose(scaleH.read(), rotH.read(), posH.read());
            //generate hull
            auto physicsCompHandle = ent.add_component<physicsComponent>();
            auto physicsComp = physicsCompHandle.read();
            auto meshFilter = ent.read_component<mesh_filter>();
            auto convexCollider = physicsComp.ConstructConvexHull(meshFilter);
            physicsCompHandle.write(physicsComp);

            //add rigidbody 
            auto posHandle = ent.get_component_handle<position>();
            auto rbH = ent.add_component<rigidbody>();
            auto fragmentRB = rbH.read();
            fragmentRB.globalCentreOfMass = posH.read();

            //add force based on distance from explosion point
            math::vec3 distanceFromCentroid = posH.read() - fractureParams.explosionCentroid ;
            math::vec3 forceDir = math::normalize(distanceFromCentroid);
            float forceAmount = (1.0f / (math::length(distanceFromCentroid))) * fractureParams.strength;

            //crude estimation of explosion point
            float smallestDot = std::numeric_limits<float>::max();
            HalfEdgeFace* chosenFace = nullptr;

            for (auto face : convexCollider->GetHalfEdgeFaces())
            {
                float currentDot = math::dot(forceDir, face->normal);

                if (currentDot < smallestDot)
                {
                    smallestDot = currentDot;
                    chosenFace = face;
                }
            }

            math::vec3 explosionPoint = trans * math::vec4(chosenFace->centroid, 1);

            fragmentRB.addForceAt(explosionPoint,forceDir * forceAmount);
            rbH.write(fragmentRB);
           
            colliderIter++;
        }

        registry->destroyEntity(ownerEntity );


        fractureCount++;
    }


    void Fracturer::GetVoronoiPoints(std::vector<std::vector<math::vec3>>& groupedPoints,
        std::vector<math::vec3>& voronoiPoints,math::vec3 min,math::vec3 max)
    {
        time::timer tick;

        auto vectorList = PhysicsStatics::GenerateVoronoi(voronoiPoints, min.x, max.x, min.y, max.y, min.z, max.z, 1, 1, 1);

        vectorList.pop_back();

        //groupedPoints.reserve( voronoiPoints.size() );

        for (std::vector<math::vec4>& vector : vectorList)
        {
            for (const math::vec4& position : vector)
            {
                int id = position.w;

                //log::debug("position {} id {} ",math::to_string(math::vec3(position)), id);
                groupedPoints.at(id).push_back(position);

            }
        }

    }

    void Fracturer::InstantiateVoronoiColliders(std::vector<std::shared_ptr<ConvexCollider>>& voronoiColliders
        ,std::vector<std::vector<math::vec3>>& groupedPoints)
    {
        time::timer tick;
        int i = 1;

        for (std::vector<math::vec3>& vector : groupedPoints)
        {
            //// if (i != 1) { continue; }
            //math::color debugColor =
            //    math::color(math::linearRand(0.0f, 0.3f), math::linearRand(0.0f, 0.3f), math::linearRand(0.0f, 0.3f));
            //// i * 2.0f, 0, 0
            math::vec3 debugOffset = math::vec3();
            //math::mat4 transform =
            //    math::compose(math::vec3(1.0f), math::identity<math::quat>(), debugOffset);

            for (math::vec3 vertex : vector)
            {
                //
                math::vec3 vertPos = vertex + debugOffset;
                //debug::user_projectDrawLine(vertPos, vertPos + math::vec3(0,0.5,0), debugColor,10.0f,FLT_MAX);
            }

            auto newCollider = std::make_shared<ConvexCollider>();

            debugVectorcolliders.push_back(newCollider);
            voronoiColliders.push_back(newCollider);

            verticesList.push_back(vector);

            newCollider->ConstructConvexHullWithVertices(vector);


            //transforms.push_back(transform);
            //newCollider->DrawColliderRepresentation(transform,math::colors::green,6.0f,FLT_MAX);
            i++;
        }
    }

    void Fracturer::GenerateFractureFragments(std::vector<ecs::entity_handle>& entitiesGenerated
        , std::vector< FracturerColliderToMeshPairing>& colliderToMeshPairings
        ,std::vector< std::shared_ptr<ConvexCollider>>& voronoiColliders
        ,ecs::entity_handle fracturedEnt)
    {
        
        fast_time totalMeshSplitting = 0;

        fast_time totalCollisionDetection = 0;

        int fractureID = 0;
        //for each instantiated convex collider
        for (std::shared_ptr<ConvexCollider> instantiatedVoronoiCollider : voronoiColliders)
        {
            for (auto& meshToColliderPairing : colliderToMeshPairings)
            {
                //check if it collides with one of the colliders in the original physics component
                physics_manifold manifold;
                ConvexConvexCollisionInfo collisionInfo;


                auto ownerEntity = meshToColliderPairing.meshSplitterPairing.
                    entity;
                auto [posH, rotH, scaleH] = ownerEntity.get_component_handles<transform>();

                auto transformB = math::compose(scaleH.read(), rotH.read(), posH.read());

                time::timer convexConvexCollision;
 /*               PhysicsStatics::DetectConvexConvexCollision(instantiatedVoronoiCollider.get()
                    , meshToColliderPairing.colliderPair.get(), math::mat4(1.0f), transformB, collisionInfo, manifold);*/
                manifold.isColliding = true;
                totalCollisionDetection += convexConvexCollision.elapsedTime().milliseconds();

                if (manifold.isColliding)
                {
                    if (fractureID < 999) //2
                    {
                        //log::debug("-> Collision Found");

                        std::vector<MeshSplitParams> splittingParams;
                        meshToColliderPairing.GenerateSplittingParamsFromCollider(instantiatedVoronoiCollider, splittingParams);
                        //log::debug("splittingParams {} ", splittingParams.size());

                        for (size_t i = 0; i < splittingParams.size(); i++)
                        {
                            float interpolant = (float)i / splittingParams.size();

                            math::vec3 color = math::color(1, 0, 0) * interpolant;

                            /*debug::user_projectDrawLine(splittingParams.at(i).planePostion
                                , splittingParams.at(i).planePostion + splittingParams.at(i).planeNormal,
                                math::color(color.x, color.y, color.z, 1), 15.0f, FLT_MAX, true);*/
                        }

                        auto splitter = meshToColliderPairing.meshSplitterPairing.read();
                        time::timer meshSplitting;
                        splitter.MultipleSplitMesh(splittingParams, entitiesGenerated, true, -1);
                        totalMeshSplitting += meshSplitting.elapsedTime().milliseconds();

                        meshToColliderPairing.meshSplitterPairing.write(splitter);

                    }

                }
                fractureID++;

            }
        }

        registry->destroyEntity(fracturedEnt);

       
    }

    void Fracturer::QuadrantVoronoi(math::vec3& min,math::vec3& max, std::vector<math::vec3>& voronoiPoints)
    {
        math::vec3 difference = max - min;
        math::vec3 differenceQuadrant = difference / 4.0f;

        math::vec3 first = min + differenceQuadrant;
        voronoiPoints.push_back(first);

        math::vec3 second = max - differenceQuadrant;
        voronoiPoints.push_back(second);

        math::vec3 third = max - (differenceQuadrant * 2);
        voronoiPoints.push_back(third);

        math::vec3 fourth = third + math::vec3(0.2f, 0, 0);
        voronoiPoints.push_back(fourth);

        math::vec3 fifth = third + math::vec3(0, -0.1f, 0);
        voronoiPoints.push_back(fifth);

        //math::vec3 sixth = min + differenceQuadrant + math::vec3(0.1, 0.25f, 0.1);
        //voronoiPoints.push_back(sixth);

        math::vec3 centroid = (min + max) / 2.0f;
        int rand = math::linearRand(0, 5);
        //log::debug("rand {} ",rand );
        for (math::vec3& point : voronoiPoints)
        {
            math::vec3 vecFromCentroid = point - centroid;

            vecFromCentroid = math::rotateY(vecFromCentroid, math::deg2rad(90.0f * rand));

            point = centroid + vecFromCentroid;

        }

    }

    void Fracturer::BalancedVoronoi(math::vec3& min, math::vec3& max, std::vector<math::vec3>& voronoiPoints)
    {
        math::vec3 difference = max - min;

        math::vec3 first = min + difference * 0.1f;
        voronoiPoints.push_back(first);
    }

    bool Fracturer::IsFractureConditionMet(physics_manifold& manifold, bool isfracturingA)
    {
        //TODO the user should be able to create their own fracture condition, but for now
        //, limit it so something can only be fractured once

        auto fractureInstigator = isfracturingA ? manifold.entityB : manifold.entityA;

        bool isAtMomentumThreshold = false;

        auto rbH =fractureInstigator.get_component_handle<rigidbody>();

        if (rbH)
        {
            auto rb = rbH.read();
            auto mass = (1.0f / rb.inverseMass);
            isAtMomentumThreshold = math::length(rbH.read().velocity * mass) > 5.0f;
        }

        return fractureCount == 0 && isAtMomentumThreshold;
    }

    void Fracturer::InitializeVoronoi(ecs::component<physicsComponent> physicsComponent)
    {
        //using all colliders inside the physicsComponent, create a bounding box 

    }

    void Fracturer::InvestigateColliderToMeshPairing(ecs::entity_handle ent, std::vector<FracturerColliderToMeshPairing> colliderToMeshPairings)
    {
        ecs::component<mesh_filter> meshFilterHandle = ent.get_component_handle<mesh_filter>();
        ecs::component<physicsComponent> physicsComponentHandle = ent.get_component_handle<physicsComponent>();

        if (!meshFilterHandle || !physicsComponentHandle) { return; }
    }

    void Fracturer::InstantiateColliderMeshPairingWithEntity(ecs::entity_handle ent,
        std::vector<FracturerColliderToMeshPairing>& colliderToMeshPairings)
    {
        //the fracturer is setup in a way that fragments are all stored in a different entity
        auto physicsCompHandle = ent.get_component_handle<physics::physicsComponent>();
        std::shared_ptr<ConvexCollider> convexCollider = nullptr;
        assert(physicsCompHandle);

        auto physicsCollider = physicsCompHandle.read().colliders.at(0);

        if (physicsCompHandle)
        {
            convexCollider = std::dynamic_pointer_cast<ConvexCollider>(physicsCollider);
        }

        auto meshSplitterHandle = ent.get_component_handle<MeshSplitter>();
        //assert(meshSplitterHandle);

        if (meshSplitterHandle && convexCollider)
        {
            FracturerColliderToMeshPairing colliderToMeshPairing(convexCollider, meshSplitterHandle);
            colliderToMeshPairings.push_back(colliderToMeshPairing);
        }


    }

    math::vec3 Fracturer::GetImpactPointFromManifold(physics_manifold& manifold)
    {
        math::vec3 impactPoint = math::vec3();
   
        for (auto& contact : manifold.contacts)
        {
            impactPoint += contact.RefWorldContact;
            impactPoint += contact.IncWorldContact;
        }

        float numberOfContacts = manifold.contacts.size() * 2;
        float mult = (1.0f / numberOfContacts);

        impactPoint *= mult;
        return impactPoint;
    }

    void FracturerColliderToMeshPairing::GenerateSplittingParamsFromCollider(std::shared_ptr<ConvexCollider> instantiatedCollider, std::vector<physics::MeshSplitParams>& meshSplitParams)
    {
        int until = 5;//convex bug at 3
        int count = 0;
        for (auto face : instantiatedCollider->GetHalfEdgeFaces())
        {
            //if (count > until) { continue; }
            MeshSplitParams splitParam(face->centroid, face->normal);
            meshSplitParams.push_back(splitParam);
            count++;
        }

    }

}
