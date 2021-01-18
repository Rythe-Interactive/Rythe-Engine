#include <physics/components/fracturer.h>
#include <physics/data/physics_manifold.hpp>
#include <physics/physics_statics.hpp>
namespace legion::physics
{
    void Fracturer::HandleFracture(physics_manifold& manifold, bool& manifoldValid,bool isfracturingA)
    {
        if (!IsFractureConditionMet()) { return; }
        log::debug("manifold invalidated");
        manifoldValid = false;


        //create a voronoi diagram with a  set of positions for now the diagram is always as big as the colliders of the physicsComponent//

        auto collider = isfracturingA ? manifold.colliderA: manifold.colliderB;
        auto fractureInstigatorEnt = isfracturingA ? manifold.physicsCompB.entity : manifold.physicsCompA.entity;

        auto [min,max] = collider->GetMinMaxWorldAABB();

        math::vec3 difference = max - min;
        math::vec3 differenceQuadrant = difference / 4.0f;

        //create voronoi points

        std::vector<math::vec3> VoronoiPoints;

        math::vec3 first = min + differenceQuadrant;
        VoronoiPoints.push_back(first);

        math::vec3 second = max - differenceQuadrant;
        VoronoiPoints.push_back(second);

        math::vec3 third = max - (differenceQuadrant * 2);
        VoronoiPoints.push_back(third);

        for (auto point : VoronoiPoints)
        {
            debug::user_projectDrawLine(point,
                point + math::vec3(0, 0.5f, 0), math::colors::magenta, 8.0f, FLT_MAX, true);
        }

        auto vectorList = PhysicsStatics::GenerateVoronoi(VoronoiPoints, min.x, max.x, min.y, max.y, min.z, max.z,1,1,1);

        vectorList.pop_back();

        debug::user_projectDrawLine(min,
            min + math::vec3(0, 0.5f, 0), math::colors::red, 8.0f, FLT_MAX, true);

        debug::user_projectDrawLine(max,
            max + math::vec3(0, 0.5f, 0), math::colors::blue, 8.0f, FLT_MAX, true);

        std::vector<std::vector<math::vec3>> groupedPoints(VoronoiPoints.size());



        for (std::vector<math::vec4>& vector : vectorList)
        {
            for (const math::vec4& position : vector)
            {
                int id = position.w;

                log::debug("position {} id {} ",math::to_string(math::vec3(position)), id);
                groupedPoints.at(id).push_back(position);

            }
        }

        

        //using positions of voronoi diagram create an array of convex colliders with quickhull
        std::vector<std::shared_ptr<ConvexCollider>> voronoiColliders;
        int i = 1;

        for ( std::vector<math::vec3>& vector : groupedPoints)
        {
           // if (i != 1) { continue; }
            math::color debugColor = 
                math::color(math::linearRand(0.0f, 0.3f), math::linearRand(0.0f, 0.3f), math::linearRand(0.0f, 0.3f));

            math::mat4 transform =
                math::compose(math::vec3(1.0f), math::identity<math::quat>(), math::vec3(i * 2.0f, 0, 0));

            for (math::vec3 vertex : vector)
            {
                math::vec3 vertPos = vertex + math::vec3(i * 2.0f, 0, 0);
                debug::user_projectDrawLine(vertPos, vertPos + math::vec3(0,0.5,0), debugColor,10.0f,FLT_MAX);
            }

            auto newCollider = std::make_shared<ConvexCollider>();
            colliders.push_back(newCollider);
            verticesList.push_back(vector);
            newCollider->ConstructConvexHullWithVertices(vector);


            transforms.push_back(transform);
            newCollider->DrawColliderRepresentation(transform,math::colors::green,4.0f,FLT_MAX);
            i++;
        }


        std::vector< FracturerColliderToMeshPairing> colliderToMeshPairings;

        //get list of collider to mesh pairs

        //make sure fractureInstigatorEnt is a direct child of the world
        
        for (size_t i = 0; i < fractureInstigatorEnt.child_count() ; i++)
        {
            //get mesh
            //get mesh_splitter
            //get physicsComponent

            //if all three components are present in this child added in colliderToMeshPairings
        }


        //for each instantiated convex collider

            //check if it collides with one of the colliders in the original physics component
            //if it does
                //push back to pair list

        //for each pair list

            //invalidate original collider

            //for each face in instantiated Collider
                //split associated mesh with splitting params
                //run quick hull on new mesh


        //for each generated collider paired with a newly generated mesh
            //if the collider is in the impact sphere, create a new entity
                
            //else,add it back in the original collider
                
        fractureCount++;
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

}
