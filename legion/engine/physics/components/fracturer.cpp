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
        if (!IsFractureConditionMet() || manifold.contacts.empty()) { return; }
       

        //log::debug("manifold invalidated");
        manifoldValid = false;

        //-----------------------------------------------------------------------------------------------------------------------------//
        //------------------------ create a voronoi diagram with a  set of positions for now the diagram-------------------------------//
        //------------------------------- is always as big as the colliders of the physicsComponent------------------------------------//
        //-----------------------------------------------------------------------------------------------------------------------------//

        auto collider = isfracturingA ? manifold.colliderA: manifold.colliderB;
        auto fracturedEnt = isfracturingA ? manifold.physicsCompA.entity : manifold.physicsCompB.entity;


        std::string dbg = isfracturingA ? "entityA is instigator" : "entityB is instigator";
        //log::debug( dbg);
        assert(fracturedEnt);

        auto [min,max] = collider->GetMinMaxWorldAABB();

        math::vec3 difference = max - min;
        math::vec3 differenceQuadrant = difference / 4.0f;

        //-----------------------------------------------------------------------------------------------------------------------------//
                                //Generate a Voronoi Diagram, for now, the points are manually generated //
        //-----------------------------------------------------------------------------------------------------------------------------//

        std::vector<math::vec3> voronoiPoints;

        math::vec3 first = min + differenceQuadrant ;
        voronoiPoints.push_back(first);

        math::vec3 second = max - differenceQuadrant;
        voronoiPoints.push_back(second);

        math::vec3 third = max - (differenceQuadrant * 2);
        voronoiPoints.push_back(third);

        math::vec3 fourth = third + math::vec3(0.2f,0,0);
        voronoiPoints.push_back(fourth);

        math::vec3 fifth = third + math::vec3(0,-0.1f,0);
        voronoiPoints.push_back(fifth);

        //math::vec3 six = third + math::vec3(-0.2f, 0.0f, -0.0f);
        //voronoiPoints.push_back(six);

        for (auto point : voronoiPoints)
        {
            debug::drawLine(point,
                point + math::vec3(0, 0.1f, 0), math::colors::magenta, 8.0f, FLT_MAX, true);
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

        //make sure fractureInstigatorEnt is a direct child of the world, the initial instigator will always
        //be either a direct parent of the world, or a grandchild of the world

        //bool isInstigatorDirectParent = fractureInstigatorEnt.get_parent() == registry->world;
        //log::debug("isInstigatorDirectParent{} ", isInstigatorDirectParent);
        //if (!isInstigatorDirectParent)
        //{
        //    //log::debug("instigator is not direct parent");
        //    fractureInstigatorEnt = fractureInstigatorEnt.get_parent();
        //}


        InstantiateColliderMeshPairingWithEntity(fracturedEnt,
            colliderToMeshPairings);

        for (size_t i = 0; i < fracturedEnt.child_count() ; i++)
        {
            auto child = fracturedEnt.get_child(i);

            InstantiateColliderMeshPairingWithEntity(child,
                colliderToMeshPairings);
        }

        //log::debug("colliderToMeshPairings size {} ", colliderToMeshPairings.size());
        //log::debug("voronoiColliderssize {} ", voronoiColliders.size());
        std::vector<ecs::entity_handle> entitiesGenerated;

        GenerateFractureFragments(entitiesGenerated, colliderToMeshPairings,
            voronoiColliders, fracturedEnt);


        /*registry->destroyEntity(manifold.physicsCompA.entity);
        registry->destroyEntity(manifold.physicsCompB.entity);*/

        //log::debug("entities generated {} ", entitiesGenerated.size());

        math::vec3 impactPoint = GetImpactPointFromManifold(manifold);
        float impactRadius = 0.2f;

        //draw impact sphere

        debug::user_projectDrawLine
        (impactPoint, impactPoint + math::vec3(0, impactRadius, 0), math::colors::blue, 3.0f, FLT_MAX, true);
        debug::user_projectDrawLine
        (impactPoint, impactPoint + math::vec3(0, -impactRadius, 0), math::colors::blue, 3.0f, FLT_MAX, true);

        debug::user_projectDrawLine
        (impactPoint, impactPoint + math::vec3(impactRadius, 0, 0), math::colors::blue, 3.0f, FLT_MAX, true);
        debug::user_projectDrawLine
        (impactPoint, impactPoint + math::vec3(-impactRadius, 0, 0), math::colors::blue, 3.0f, FLT_MAX, true);

        debug::user_projectDrawLine
        (impactPoint, impactPoint + math::vec3(0, 0, impactRadius), math::colors::blue, 3.0f, FLT_MAX, true);
        debug::user_projectDrawLine
        (impactPoint, impactPoint + math::vec3(0, 0, -impactRadius), math::colors::blue, 3.0f, FLT_MAX, true);



        int colliderIter = 0;
        for (auto ent : entitiesGenerated)
        {
            auto physicsCompHandle = ent.add_component<physicsComponent>();
            auto physicsComp = physicsCompHandle.read();

            auto meshFilter = ent.read_component<mesh_filter>();

            auto vertices = meshFilter.get().second.vertices;

            //log::debug("cch");
            bool debug = false;
            //log::debug("debug {} ", debug);
            auto convexCollider = physicsComp.ConstructConvexHull(meshFilter, debug);

            auto posHandle = ent.get_component_handle<position>();
            /* debug::user_projectDrawLine(posHandle.read(),
                 posHandle.read() + math::vec3(0,0.2f,0),math::colors::red,15.0f,FLT_MAX,true);*/
            ent.add_component<rigidbody>();

            auto [posH, rotH,scaleH] = ent.get_component_handles<transform>();
            math::mat4 colliderTransform = math::compose(scaleH.read(), rotH.read(), posH.read());

            float seperation;
            if (PhysicsStatics::DetectConvexSphereCollision(convexCollider.get(),
                colliderTransform, impactPoint, impactRadius, seperation))
            {
                /*debug::drawLine(posH.read(),
                    posH.read() + math::vec3(0, 0.2f, 0), math::colors::red, 15.0f, FLT_MAX, true);*/
                //registry->destroyEntity(ent);
                //log::debug("sphere collision detected");
            }

            colliderIter++;
        }

        //registry->destroyEntity(fractureInstigatorEnt);

        //log::debug("all fragments have convex hulls");
        //for each pair list
            

             //invalidate original collider


        //for each generated collider paired with a newly generated mesh
            //if the collider is in the impact sphere, create a new entity
                
            //else,add it back in the original collider
                
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

        log::debug("GetVoronoiPoints {} ms ", tick.elapsedTime().milliseconds());

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

        log::debug("InstantiateVoronoiColliders {} ms ", tick.elapsedTime().milliseconds());

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
                    if (fractureID < 9) //2
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

                        //if (fractureID == 1)
                        //{
                        //    auto ent = entitiesGenerated.back();
                        //    auto idH = ent.add_component<identifier>();
                        //    auto id = idH.read();
                        //    id.id = "problem";
                        //    idH.write(id);

                        //}


                    }

                }
                fractureID++;

            }
        }

        registry->destroyEntity(fracturedEnt);

        log::debug("total Mesh Splitting {} ", totalMeshSplitting);
        log::debug("total collision detection {} ", totalCollisionDetection);
    }

    bool Fracturer::IsFractureConditionMet()
    {
        //TODO the user should be able to create their own fracture condition, but for now
        //, limit it so something can only be fractured once
        log::debug("fractureCount {} ", fractureCount);
        return fractureCount == 0;
    }

    void Fracturer::InitializeVoronoi(ecs::component_handle<physicsComponent> physicsComponent)
    {
        //using all colliders inside the physicsComponent, create a bounding box 

    }

    void Fracturer::InvestigateColliderToMeshPairing(ecs::entity_handle ent, std::vector<FracturerColliderToMeshPairing> colliderToMeshPairings)
    {
        ecs::component_handle<mesh_filter> meshFilterHandle = ent.get_component_handle<mesh_filter>();
        ecs::component_handle<physicsComponent> physicsComponentHandle = ent.get_component_handle<physicsComponent>();

        if (!meshFilterHandle || !physicsComponentHandle) { return; }
    }

    void Fracturer::InstantiateColliderMeshPairingWithEntity(ecs::entity_handle ent,
        std::vector<FracturerColliderToMeshPairing>& colliderToMeshPairings)
    {
        //the fracturer is setup in a way that fragments are all stored in a different entity
        auto physicsCompHandle = ent.get_component_handle<physics::physicsComponent>();
        std::shared_ptr<ConvexCollider> convexCollider = nullptr;
        assert(physicsCompHandle);

        auto physicsComp = physicsCompHandle.read();
        auto physicsCollider = physicsComp.colliders->at(0);

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
        //log::debug("impactPoint {} ", math::to_string(impactPoint));

        float numberOfContacts = manifold.contacts.size() * 2;
        //log::debug("numberOfContacts {} ", numberOfContacts);
        float mult = (1.0f / numberOfContacts);

        //TODO doing this the normal way triggers an illegal operation somehow, find out why
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
