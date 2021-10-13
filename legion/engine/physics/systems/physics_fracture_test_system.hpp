#pragma once
#include <core/core.hpp>
#include <rendering/components/renderable.hpp>
#include <physics/cube_collider_params.hpp>
using namespace legion;

struct extendedPhysicsContinue : public app::input_action<extendedPhysicsContinue> {};
struct nextPhysicsTimeStepContinue : public app::input_action<nextPhysicsTimeStepContinue> {};
struct physics_split_test : public app::input_action<physics_split_test> {};
struct spawnEntity : public app::input_action<spawnEntity> {};

struct smallExplosion : public app::input_action<smallExplosion> {};
struct mediumExplosion : public app::input_action<mediumExplosion> {};
struct largeExplosion : public app::input_action<largeExplosion> {};

struct explosion : public app::input_action<explosion> {};


namespace legion::physics
{
    class PhysicsFractureTestSystem final : public System<PhysicsFractureTestSystem>
    {
    public:

        virtual void setup();

        virtual void colliderDraw(time::span dt);

    private:

        void CreateElongatedFloor(math::vec3 position,math::quat rot, math::vec3 scale, rendering::material_handle mat, bool hasCollider =true);

        ecs::entity_handle CreateSplitTestBox(physics::cube_collider_params cubeParams, math::vec3 position,
            math::quat rotation, rendering::material_handle mat, bool isFracturable, bool hasRigidbody = false
            , math::vec3 velocity = math::vec3(), float explosionStrength = 0.0f, float explosionTime = FLT_MAX,
            math::vec3 impactPoint = math::vec3(-69.0f,0.0f,0.0f), bool hasCollider = true);

        void OnSplit(physics_split_test* action);

        void meshSplittingTest(rendering::model_handle planeH, rendering::model_handle cubeH
            , rendering::model_handle cylinderH, rendering::model_handle complexH, rendering::material_handle TextureH);

        void collisionResolutionTest(rendering::model_handle cubeH,
            rendering::material_handle wireframeH);

        void fractureVideoScene();

        void prematureExplosion(explosion* action);

        void numericalRobustnessTest();

        void extendedContinuePhysics(extendedPhysicsContinue * action);

        void OneTimeContinuePhysics(nextPhysicsTimeStepContinue * action);

        void spawnCubeStack(math::vec3 start);

        void compositeColliderTest();

        void explosionTest();

        void spawnEntityOnCameraForward(spawnEntity * action);

        void simpleMinecraftHouse(math::vec3 start);

        void createFloor(int xCount, int yCount, math::vec3 start,
            math::vec3 offset, rendering::model_handle cubeH, std::vector< rendering::material_handle> materials
            , std::vector<int> ignoreJ, std::vector<bool> shouldFracture , float fractureTime = FLT_MAX,
            math::vec3 impactPoint = math::vec3(), bool hasRigidbodies = false,float strength =0.0f,bool hasCollider = true);

        void smallExplosionTest(smallExplosion*action);
        void mediumExplosionTest(mediumExplosion*action);
        void largeExplosionTest(largeExplosion*action);

        void explodeAThing(time::span);

        enum explosionType : int
        {
            NO_BOOM = 0,
            SMALL_BOOM = 1,
            MEDIUM_BOOM = 2,
            BIG_BOOM = 4,
        };

        explosionType m_boom = explosionType::NO_BOOM;

        void fractureTest();
        rendering::material_handle textureH;
        rendering::material_handle woodTextureH;
        rendering::material_handle rockTextureH;
        rendering::material_handle concreteH;
        rendering::material_handle tileH;
        rendering::material_handle directionalLightMH;
        rendering::material_handle brickH;

        rendering::model_handle cubeH;
        rendering::model_handle concaveTestObject;
        rendering::model_handle planeH;
        rendering::model_handle cylinderH;
        rendering::model_handle complexH;
        rendering::model_handle directionalLightH;

        ecs::entity_handle smallExplosionEnt;
        ecs::entity_handle mediumExplosionEnt;
        ecs::entity_handle largeExplosionEnt;

        ecs::entity_handle staticToAABBEntLinear, staticToAABBEntRotation, staticToOBBEnt, staticToEdgeEnt;

    };
}
