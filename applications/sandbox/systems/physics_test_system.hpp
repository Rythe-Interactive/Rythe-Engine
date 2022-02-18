#pragma once
#include <core/core.hpp>
#include <rendering/components/renderable.hpp>
#include <physics/diviner/cube_collider_params.hpp>

namespace lgn = legion;

struct extendedPhysicsContinue : public lgn::app::input_action<extendedPhysicsContinue> {};
struct nextPhysicsTimeStepContinue : public lgn::app::input_action<nextPhysicsTimeStepContinue> {};

struct QHULL : public lgn::app::input_action<QHULL>{};
struct AddRigidbody : public lgn::app::input_action<AddRigidbody> {};
struct SpawnRandomHullOnCameraLoc : public lgn::app::input_action< SpawnRandomHullOnCameraLoc> {};
struct SpawnHullActive : public lgn::app::input_action< SpawnHullActive> {};

struct ObjectToFollow
{
    lgn::ecs::entity ent;
};

namespace legion::physics
{
    class PhysicsTestSystem final : public System<PhysicsTestSystem>
    {
    public:

        virtual void setup();

        virtual void colliderDraw(time::span dt);

    private:

        bool m_throwingHullActivated = false;

        int m_step = 0;
        int m_maxStep = 0;

        //SCENES

        void initializeLitMaterial(rendering::material_handle& materialToInitialize, rendering::shader_handle& litShader,
            const fs::view& albedoFile, const fs::view& normalFile, const fs::view& roughnessFile);

        void quickhullTestScene();

        void BoxStackScene();

        void stabilityComparisonScene();

        void monkeyStackScene();

        //SCENE HELPERS

        void createQuickhullTestObject(math::vec3 position, rendering::model_handle cubeH, rendering::material_handle TextureH,math::mat3 inertia = math::mat3(6.0f));

        void PopulateFollowerList(ecs::entity physicsEnt,int index);
       
        void addStaircase(math::vec3 position,float breadthMult = 1.0f,float widthMult = 27.0f);

        //FUNCTION BINDED ACTIONS

        void ActivateSpawnRandomHull(SpawnHullActive&action);

        void spawnRandomConvexHullOnCameraLocation(SpawnRandomHullOnCameraLoc&action);

        void extendedContinuePhysics(extendedPhysicsContinue& action);

        void OneTimeContinuePhysics(nextPhysicsTimeStepContinue& action);

        void quickHullStep(QHULL& action);

        void AddRigidbodyToQuickhulls(AddRigidbody& action);

        void drawPhysicsColliders();

        void createStack(int widthCount, int breadthCount, int heightCount,
            math::vec3 firstBlockPos, math::vec3 offset, rendering::model_handle cubeH,
            rendering::material_handle materials, physics::cube_collider_params cubeParams,
            bool useQuickhull = false, bool rigidbody = true, float mass = 1.0f, math::mat3 inverseInertia = math::mat3(6.f));

        void createBoxEntity(math::vec3 position, rendering::model_handle cubeH,
            rendering::material_handle materials, physics::cube_collider_params cubeParams,
            bool useQuickhull = false, bool rigidbody = true, float mass = 1.0f, math::mat3 inverseInertia = math::mat3(6.f));

        rendering::material_handle textureH;
        rendering::material_handle woodTextureH;
        rendering::material_handle rockTextureH;
        rendering::material_handle concreteH;
        rendering::material_handle tileH;
        rendering::material_handle directionalLightMH;
        rendering::material_handle vertexColor;
        rendering::material_handle brickH;
        rendering::material_handle wireFrameH;
   
        rendering::model_handle cubeH;
        rendering::model_handle concaveTestObject;
        rendering::model_handle planeH;
        rendering::model_handle cylinderH;
        rendering::model_handle complexH;
        rendering::model_handle directionalLightH;
   
        //convex hull tests
        rendering::model_handle colaH;
        rendering::model_handle hammerH;
        rendering::model_handle suzzaneH;
        rendering::model_handle teapotH;

        ecs::entity staticToAABBEntLinear, staticToAABBEntRotation, staticToOBBEnt, staticToEdgeEnt;


        std::vector< ecs::entity> registeredColliderColorDraw;

        std::vector<std::vector<ecs::entity>> folowerObjects;
    };
}
