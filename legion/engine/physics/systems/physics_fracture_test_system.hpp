#pragma once
#include <core/core.hpp>
#include <rendering/components/renderable.hpp>
#include <physics/cube_collider_params.hpp>
using namespace legion;

struct extendedPhysicsContinue : public app::input_action<extendedPhysicsContinue> {};
struct nextPhysicsTimeStepContinue : public app::input_action<nextPhysicsTimeStepContinue> {};
struct physics_split_test : public app::input_action<physics_split_test> {};

namespace legion::physics
{
    class PhysicsFractureTestSystem final : public System<PhysicsFractureTestSystem>
    {
    public:

        virtual void setup();

        virtual void colliderDraw(time::span dt);

    private:

        void CreateElongatedFloor(math::vec3 position,math::quat rot, math::vec3 scale);

        void CreateSplitTestBox(physics::cube_collider_params cubeParams, math::vec3 position,
            math::quat rotation, bool isFracturable);

        void OnSplit(physics_split_test* action);

        void meshSplittingTest(rendering::model_handle planeH, rendering::model_handle cubeH
            , rendering::model_handle cylinderH, rendering::model_handle complexH, rendering::material_handle TextureH);

        void collisionResolutionTest(rendering::model_handle cubeH,
            rendering::material_handle wireframeH);

        void numericalRobustnessTest();

        void extendedContinuePhysics(extendedPhysicsContinue * action);

        void OneTimeContinuePhysics(nextPhysicsTimeStepContinue * action);

        void compositeColliderTest();
            
        void fractureTest();
        rendering::material_handle textureH;
        rendering::material_handle woodTextureH;

        rendering::model_handle cubeH;
        rendering::model_handle concaveTestObject;
        rendering::model_handle planeH;
        rendering::model_handle cylinderH;
        rendering::model_handle complexH;


        ecs::entity_handle staticToAABBEntLinear, staticToAABBEntRotation, staticToOBBEnt, staticToEdgeEnt;

    };

  

}
