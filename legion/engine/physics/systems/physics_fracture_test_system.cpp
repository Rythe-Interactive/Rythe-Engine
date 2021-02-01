#include <physics/systems/physics_fracture_test_system.hpp>
#include <physics/mesh_splitter_utils/mesh_splitter.hpp>
#include <physics/mesh_splitter_utils/splittable_polygon.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/systems/physicssystem.hpp>
#include <physics/components/fracturer.hpp>
#include <rendering/debugrendering.hpp>
#include <rendering/components/camera.hpp>

namespace legion::physics
{

    void PhysicsFractureTestSystem::setup()
    {
        using namespace legion::core::fs::literals;

        physics::PrimitiveMesh::SetECSRegistry(m_ecs);

        auto win = world.read_component<app::window>();

        app::context_guard guard(win);

        #pragma region Material Setup
        textureH = rendering::MaterialCache::create_material("texture", "assets://shaders/texture.shs"_view);
        textureH.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/split-test.png"_view));

        woodTextureH = rendering::MaterialCache::create_material("texture2", "assets://shaders/texture.shs"_view);
        woodTextureH.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/log.jpg"_view));

        rockTextureH = rendering::MaterialCache::create_material("rock", "assets://shaders/texture.shs"_view);
        rockTextureH.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/rock.png"_view));
        #pragma endregion

        #pragma region Model Setup
        cubeH = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
        planeH = rendering::ModelCache::create_model("plane", "assets://models/plane.obj"_view);
        cylinderH = rendering::ModelCache::create_model("cylinder", "assets://models/cylinder.obj"_view);
        concaveTestObject = rendering::ModelCache::create_model("concaveTestObject", "assets://models/polygonTest.obj"_view);
        #pragma endregion

        #pragma region Input binding
        app::InputSystem::createBinding<physics_split_test>(app::inputmap::method::ENTER);
        app::InputSystem::createBinding<extendedPhysicsContinue>(app::inputmap::method::M);
        app::InputSystem::createBinding<nextPhysicsTimeStepContinue>(app::inputmap::method::N);
        app::InputSystem::createBinding<spawnEntity>(app::inputmap::method::MOUSE_LEFT);

        app::InputSystem::createBinding<smallExplosion>(app::inputmap::method::NUM1);
        app::InputSystem::createBinding<mediumExplosion>(app::inputmap::method::NUM2);
        app::InputSystem::createBinding<largeExplosion>(app::inputmap::method::NUM3);
      
        /*app::InputSystem::createBinding< activate_CRtest2>(app::inputmap::method::KP_2);
        app::InputSystem::createBinding< activate_CRtest3>(app::inputmap::method::KP_3);*/
        #pragma endregion

        #pragma region Function binding
        /*bindToEvent< activate_CRtest2, &TestSystem::onActivateUnitTest2>();
        bindToEvent< activate_CRtest3, &TestSystem::onActivateUnitTest3>();*/
        bindToEvent<physics_split_test,&PhysicsFractureTestSystem::OnSplit > ();
        bindToEvent<extendedPhysicsContinue, &PhysicsFractureTestSystem::extendedContinuePhysics>();
        bindToEvent<nextPhysicsTimeStepContinue, &PhysicsFractureTestSystem::OneTimeContinuePhysics>();
        bindToEvent<spawnEntity, &PhysicsFractureTestSystem::spawnEntityOnCameraForward>();

        bindToEvent<smallExplosion, &PhysicsFractureTestSystem::smallExplosionTest>();
        bindToEvent<mediumExplosion, &PhysicsFractureTestSystem::mediumExplosionTest>();
        bindToEvent<largeExplosion, &PhysicsFractureTestSystem::largeExplosionTest>();

        #pragma endregion

        //compositeColliderTest();
        //fractureTest();
        //numericalRobustnessTest();
        //simpleMinecraftHouse();
        explosionTest();
        /*meshSplittingTest(planeH, cubeH
            , cylinderH, complexH, textureH);*/

        createProcess<&PhysicsFractureTestSystem::colliderDraw>("Update");
        createProcess<&PhysicsFractureTestSystem::explodeAThing>("Physics");

        Fracturer::registry = m_ecs;
    }

    void PhysicsFractureTestSystem::colliderDraw(time::span dt)
    {

        //static ecs::EntityQuery halfEdgeQuery = createQuery<physics::MeshSplitter,transform>();
        //halfEdgeQuery.queryEntities();

        //for (auto entity : halfEdgeQuery)
        //{
        //    auto edgeFinderH = entity.get_component_handle<physics::MeshSplitter>();
        //    auto edgeFinder = edgeFinderH.read();
       
        //    auto transH = entity.get_component_handles<transform>();
        //    
        //    auto [posH, rotH, scaleH] = entity.get_component_handles<transform>();
        //    const math::mat4 transform = math::compose(scaleH.read(), rotH.read(), posH.read());

        //    for (auto pol : edgeFinder.meshPolygons)
        //    {
        //        const math::vec3& worldCentroid = transform * math::vec4(pol->localCentroid, 1);
        //        const math::vec3& worldNormal = transform * math::vec4(pol->localNormal, 0);

        //        for (auto edge : pol->GetMeshEdges())
        //        {
        //            if (edge->isBoundary)
        //            {
        //                auto [start, end] = edge->getEdgeWorldPositions(transform);
        //                auto startOffset = (worldCentroid - start) * 0.1f + worldNormal * 0.01f;
        //                auto endOffset = (worldCentroid - end) * 0.1f + worldNormal * 0.01f;
        //                
        //                debug::user_projectDrawLine(start + startOffset, end + endOffset, pol->debugColor, 5.0f);
        //            }
        //            
        //        }
        //    }

        //   

        //    for (auto pol : edgeFinder.meshPolygons)
        //    {
        //        int boundaryCount = pol->CountBoundary();


        //        if (boundaryCount == 16)
        //        {
        //            math::vec3 worldCentroid = transform * math::vec4(pol->localCentroid, 1);

        //            //
        //            debug::user_projectDrawLine(worldCentroid, worldCentroid  + math::vec3(0, 0.1, 0), math::colors::red, 5.0f, 30.0f);
        //        }

        //        //log::debug(" polygon had {} boundary edges ", boundaryCount);
        //    }

        //}


    }

    void PhysicsFractureTestSystem::meshSplittingTest(rendering::model_handle planeH, rendering::model_handle cubeH
        , rendering::model_handle cylinderH, rendering::model_handle complexH, rendering::material_handle TextureH)
    {
       

        physics::cube_collider_params cubeParams;
        cubeParams.breadth = 2.0f;
        cubeParams.width = 2.0f;
        cubeParams.height = 2.0f;

        //-------------------------------------------------------------------------------------------------------------------------------//
                                                     //CUBE TEST 
        //-------------------------------------------------------------------------------------------------------------------------------//

        ecs::entity_handle cubeSplit2;
        {
            auto splitter = m_ecs->createEntity();
            cubeSplit2 = splitter;

            auto entPhyHande = splitter.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;

            physicsComponent2.AddBox(cubeParams);

            entPhyHande.write(physicsComponent2);

            splitter.add_components<rendering::mesh_renderable>(mesh_filter(planeH.get_mesh()), rendering::mesh_renderer(TextureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(splitter);
            positionH.write(math::vec3(37, 1.0f, 10.0f));
            scaleH.write(math::vec3(2.0f));

            auto rotation = rotationH.read();

            rotation *= math::angleAxis(math::deg2rad(-60.0f), math::vec3(1, 0, 0));

            splitter.write_component(rotation);

        }

        //Cube split plane
        ecs::entity_handle cubeSplit;
        {
             auto splitter = m_ecs->createEntity();
             cubeSplit = splitter;

             auto entPhyHande = splitter.add_component<physics::physicsComponent>();

             physics::physicsComponent physicsComponent2;

             physicsComponent2.AddBox(cubeParams);

             entPhyHande.write(physicsComponent2);

             splitter.add_components<rendering::mesh_renderable>(mesh_filter(planeH.get_mesh()), rendering::mesh_renderer(TextureH));

             auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(splitter);
             positionH.write(math::vec3(37, 1.5f, 10.0f));
             scaleH.write(math::vec3(2.0f));

             auto rotation = rotationH.read();

             //rotation *= math::angleAxis(math::deg2rad(60.0f), math::vec3(1, 0, 0));

             splitter.write_component(rotation);

        }

        //Cube 
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();
            //ent.add_component<addRB>();

            physics::physicsComponent physicsComponent2;


            physicsComponent2.AddBox(cubeParams);

            entPhyHande.write(physicsComponent2);

            ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(TextureH));
            //renderableHandle.write({ cubeH,TextureH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(37, 1.5f, 10.0f));
            scaleH.write(math::vec3(1.0f));

            auto idHandle = m_ecs->createComponent<physics::identifier>(ent);
            auto id = idHandle.read();
            id.id = "AABBRbStable";
            idHandle.write(id);

            auto finderH = ent.add_component<physics::MeshSplitter>();

            auto finder = finderH.read();
            finder.splitTester.push_back(cubeSplit);
            finder.splitTester.push_back(cubeSplit2);
            finder.InitializePolygons(ent);
            finderH.write(finder);

        }

        //-------------------------------------------------------------------------------------------------------------------------------//
                                                    //CYLINDER TEST 
        //-------------------------------------------------------------------------------------------------------------------------------//
        
        //Split plane
        ecs::entity_handle cylinderSplit;
        {
            cylinderSplit = m_ecs->createEntity();
           
            cylinderSplit.add_components<rendering::mesh_renderable>(mesh_filter(planeH.get_mesh()), rendering::mesh_renderer(TextureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(cylinderSplit);
            positionH.write(math::vec3(37, 1.3f, 15.0f));
            scaleH.write(math::vec3(2.0f));

        }

        //Cylinder
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent;

            physicsComponent.AddBox(cubeParams);
            entPhyHande.write(physicsComponent);

            ent.add_components<rendering::mesh_renderable>(mesh_filter(cylinderH.get_mesh()), rendering::mesh_renderer(TextureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(37, 1.5f, 15.0f));
            scaleH.write(math::vec3(1.0f));

            auto idHandle = m_ecs->createComponent<physics::identifier>(ent);
            auto id = idHandle.read();
            id.id = "AABBRbStable";
            idHandle.write(id);


            auto finderH = ent.add_component<physics::MeshSplitter>();

            auto finder = finderH.read();
            finder.splitTester.push_back( cylinderSplit);
            finder.InitializePolygons(ent);
            finderH.write(finder);
        }

        //-------------------------------------------------------------------------------------------------------------------------------//
                                                   //CONCAVE TEST 
        //-------------------------------------------------------------------------------------------------------------------------------//


        //Complex Mesh
        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;


            physicsComponent2.AddBox(cubeParams);

            entPhyHande.write(physicsComponent2);

            ent.add_components<rendering::mesh_renderable>(mesh_filter(concaveTestObject.get_mesh()), rendering::mesh_renderer(TextureH));


            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(37, 2.5f, 22.0f));
            scaleH.write(math::vec3(1.0f));

            auto idHandle = m_ecs->createComponent<physics::identifier>(ent);
            auto id = idHandle.read();
            id.id = "AABBRbStable";
            idHandle.write(id);

            auto finderH = ent.add_component<physics::MeshSplitter>();

            auto finder = finderH.read();
            //finder.splitTester.push_back(cylinderSplit);
            finder.InitializePolygons(ent);
            finderH.write(finder);
            log::debug("test");

            log::debug("finder size {} " , finder.meshPolygons.size());

           

        }
    }

    void PhysicsFractureTestSystem::collisionResolutionTest(rendering::model_handle cubeH, rendering::material_handle wireframeH)
    {
        physics::cube_collider_params cubeParams;
        cubeParams.breadth = 1.0f;
        cubeParams.width = 1.0f;
        cubeParams.height = 1.0f;

        physics::cube_collider_params staticBlockParams;
        staticBlockParams.breadth = 2.5f;
        staticBlockParams.width = 2.5f;
        staticBlockParams.height = 1.0f;

        float testPos = 10.0f;
        //----------- Static Block To AABB Body Stability Test ------------//
        {
            auto ent = m_ecs->createEntity();

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(testPos, -3.0f, 15.0f));
            scaleH.write(math::vec3(2.5f, 1.0f, 2.5f));

            ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(wireframeH));

        }

        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;


            physicsComponent2.AddBox(staticBlockParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);

            //auto renderableHandle = m_ecs->createComponent<rendering::mesh_renderable>(mesh_filter(ent);
            //renderableHandle.write({ cubeH), wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(testPos, -3.0f, 15.0f));
            scaleH.write(math::vec3(1.0f));

            auto idHandle = m_ecs->createComponent<physics::identifier>(ent);
            auto idComp = idHandle.read();
            idComp.id = "AABBStaticStable";
            idHandle.write(idComp);

        }

        {
            staticToAABBEntLinear = m_ecs->createEntity();
            
            auto entPhyHande = staticToAABBEntLinear.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            


            physicsComponent2.AddBox(cubeParams);
            entPhyHande.write(physicsComponent2);

            //auto crb = m_ecs->createComponent<physics::rigidbody>(staticToAABBEnt);
            //auto rbHandle = staticToAABBEnt.add_component<physics::rigidbody>();

            //auto renderableHandle = m_ecs->createComponent<rendering::mesh_renderable>(mesh_filter(staticToAABBEnt);
            //renderableHandle.write({ cubeH), wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(staticToAABBEntLinear);
            positionH.write(math::vec3(testPos, -0.0f, 15.0f));
            scaleH.write(math::vec3(1.0f));

            auto idHandle = m_ecs->createComponent<physics::identifier>(staticToAABBEntLinear);
            auto id = idHandle.read();
            id.id = "AABBRbStable";
            idHandle.write(id);
        }

        //{
        //    auto ent = m_ecs->createEntity();
        //    //physicsUnitTestCD.push_back(staticToAABBEntLinear);
        //    auto entPhyHande = ent.add_component<physics::physicsComponent>();

        //    physics::physicsComponent physicsComponent2;
        //    


        //    physicsComponent2.AddBox(cubeParams);
        //    entPhyHande.write(physicsComponent2);

        //    auto crb = m_ecs->createComponent<physics::rigidbody>(ent);
        //    auto rbHandle = ent.add_component<physics::rigidbody>();

        //    //auto renderableHandle = m_ecs->createComponent<rendering::mesh_renderable>(mesh_filter(staticToAABBEnt);
        //    //renderableHandle.write({ cubeH), wireframeH });

        //    auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
        //    positionH.write(math::vec3(testPos+0.5f, -1.0f, 15.0f));
        //    scaleH.write(math::vec3(1.0f));

        //    auto idHandle = m_ecs->createComponent<physics::identifier>(ent);
        //    auto id = idHandle.read();
        //    id.id = "AABBRbStable";
        //    idHandle.write(id);
        //}

        //----------- Static Block To AABB Body Rotation Test------------//

        {
            auto ent = m_ecs->createEntity();

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(testPos, -3.0f, 8.0f));
            scaleH.write(math::vec3(2.5f, 1.0f, 2.5f));

            ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(wireframeH));
        }

        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            
            physicsComponent2.AddBox(staticBlockParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);

            //auto renderableHandle = m_ecs->createComponent<rendering::mesh_renderable>(mesh_filter(ent);
            //renderableHandle.write({ cubeH), wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(testPos, -3.0f, 8.0f));
            scaleH.write(math::vec3(1.0f));

            auto idHandle = m_ecs->createComponent<physics::identifier>(ent);
            auto idComp = idHandle.read();
            idComp.id = "AABBStatic";
            idHandle.write(idComp);


        }

        {
            staticToAABBEntRotation = m_ecs->createEntity();
           
            auto entPhyHande = staticToAABBEntRotation.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            

            physicsComponent2.AddBox(cubeParams);
            entPhyHande.write(physicsComponent2);

            //auto crb = m_ecs->createComponent<physics::rigidbody>(staticToAABBEnt);
            //auto rbHandle = staticToAABBEnt.add_component<physics::rigidbody>();

            //auto renderableHandle = m_ecs->createComponent<rendering::mesh_renderable>(mesh_filter(staticToAABBEnt);
            //renderableHandle.write({ cubeH), wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(staticToAABBEntRotation);
            positionH.write(math::vec3(testPos - 2.7f, -0.0f, 8.0f));
            scaleH.write(math::vec3(1.0f));

            auto idHandle = m_ecs->createComponent<physics::identifier>(staticToAABBEntRotation);
            auto id = idHandle.read();
            id.id = "AABBRb";
            idHandle.write(id);
        }

        //----------- Static Block To Rotated Body ------------//

        {
            auto ent = m_ecs->createEntity();

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(testPos, -3.0f, 2.0f));
            scaleH.write(math::vec3(2.5f, 1.0f, 2.5f));

            ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(wireframeH));
        }

        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            



            physicsComponent2.AddBox(staticBlockParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);

            //auto renderableHandle = m_ecs->createComponent<rendering::mesh_renderable>(mesh_filter(ent);
            //renderableHandle.write({ cubeH), wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(testPos, -3.0f, 2.0f));
            scaleH.write(math::vec3(1.0f));

            auto idHandle = m_ecs->createComponent<physics::identifier>(ent);
            auto id = idHandle.read();
            id.id = "OBBStatic";
            idHandle.write(id);
        }

        {
            staticToOBBEnt = m_ecs->createEntity();

            auto entPhyHande = staticToOBBEnt.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            


            physicsComponent2.AddBox(cubeParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);


            //auto renderableHandle = m_ecs->createComponent<rendering::mesh_renderable>(staticToOBBEnt);
            //renderableHandle.write({ cubeH, wireframeH });
            staticToOBBEnt.add_components<rendering::mesh_renderable>
                (mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(wireframeH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(staticToOBBEnt);
            positionH.write(math::vec3(testPos, -0.0f, 2.0f));
            scaleH.write(math::vec3(1.0f));

            auto rot = rotationH.read();
            /*rot *= math::angleAxis(math::radians(90.f), math::vec3(0, 0, 1));
            rot *= math::angleAxis(math::radians(40.f), math::vec3(1, 0, 0));
            rot *= math::angleAxis(math::radians(42.f), math::vec3(0, 1, 0));*/
            //rot *= math::angleAxis(45.f, math::vec3(0, 1, 0));
            rotationH.write(rot);

            auto idHandle = m_ecs->createComponent<physics::identifier>(staticToOBBEnt);
            auto id = idHandle.read();
            id.id = "NON_STATIC";
            idHandle.write(id);


            //log::debug("rb.angularVelocity {}", rb.angularVelocity);

        }

        //----------- Static Block To Rotated Body Edge ------------//

        {
            auto ent = m_ecs->createEntity();

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(testPos, -3.0f, -4.0f));
            scaleH.write(math::vec3(2.5f, 1.0f, 2.5f));

            ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(wireframeH));
        }

        {
            auto ent = m_ecs->createEntity();

            auto entPhyHande = ent.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            



            physicsComponent2.AddBox(staticBlockParams);
            physicsComponent2.isTrigger = false;
            entPhyHande.write(physicsComponent2);

            //auto renderableHandle = m_ecs->createComponent<rendering::mesh_renderable>(mesh_filter(ent);
            //renderableHandle.write({ cubeH), wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(testPos, -3.0f, -4.0f));
            scaleH.write(math::vec3(1.0f));

            auto idHandle = m_ecs->createComponent<physics::identifier>(ent);
            auto id = idHandle.read();
            id.id = "OBBFoundationStaticEdge";
            idHandle.write(id);
        }

        {
            staticToEdgeEnt = m_ecs->createEntity();

            auto entPhyHande = staticToEdgeEnt.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            


            physicsComponent2.AddBox(cubeParams);
            entPhyHande.write(physicsComponent2);


            //auto renderableHandle = m_ecs->createComponent<rendering::mesh_renderable>(mesh_filter(staticToEdgeEnt);
            //renderableHandle.write({ cubeH), wireframeH });

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(staticToEdgeEnt);
            positionH.write(math::vec3(testPos + 3.0f, 2.0f, -4.0f));
            scaleH.write(math::vec3(1.0f));

            auto rot = rotationH.read();
            rot *= math::angleAxis(45.f, math::vec3(1, 0, 0));
            rot *= math::angleAxis(45.f, math::vec3(0, 1, 0));
            rotationH.write(rot);

            auto idHandle = m_ecs->createComponent<physics::identifier>(staticToEdgeEnt);
            auto id = idHandle.read();
            id.id = "OBBRb";
            idHandle.write(id);
            //log::debug("rb.angularVelocity {}", rb.angularVelocity);

        }
    }

    void PhysicsFractureTestSystem::extendedContinuePhysics(extendedPhysicsContinue * action)
    {
        if (action->value)
        {
            physics::PhysicsSystem::IsPaused = false;
        }
        else
        {
            physics::PhysicsSystem::IsPaused = true;
        }

    }

    void PhysicsFractureTestSystem::OneTimeContinuePhysics(nextPhysicsTimeStepContinue * action)
    {
        if (!(action->value))
        {
            physics::PhysicsSystem::IsPaused = true;
            physics::PhysicsSystem::oneTimeRunActive = true;
            log::debug(" onNextPhysicsTimeStepRequest");
        }
    }

    void PhysicsFractureTestSystem::compositeColliderTest()
    {
        physics::cube_collider_params cubeParams;
        cubeParams.breadth = 1.0f;
        cubeParams.width = 1.0f;
        cubeParams.height = 1.0f;

        //-------------------------------------------------------------------------------------------------------------------------------//
                                                        //COMPOSITE
        //-------------------------------------------------------------------------------------------------------------------------------//

        ecs::entity_handle composite;
        {
            composite = m_ecs->createEntity();


            auto entPhyHande = composite.add_component<physics::physicsComponent>();
            composite.add_component < physics::rigidbody>();

            physics::physicsComponent physicsComponent2;
            

            physicsComponent2.AddBox(cubeParams);

            physicsComponent2.AddBox(cube_collider_params(1.0f,1.0f,1.0f,math::vec3(1.0f,-1.0f,1.0f)));

            entPhyHande.write(physicsComponent2);

            composite.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(textureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(composite);
            positionH.write(math::vec3(10, 5.0f, 10.0f));
            scaleH.write(math::vec3(1.0f, 1.0f, 1.0f));

            auto rotation = rotationH.read();

            composite.write_component(rotation);

        }

        ecs::entity_handle floor;
        {
            auto ent = m_ecs->createEntity();
            floor = ent;

            auto entPhyHande = floor.add_component<physics::physicsComponent>();


            physics::physicsComponent physicsComponent2;
            

            physicsComponent2.AddBox(cubeParams);

            entPhyHande.write(physicsComponent2);

            ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(textureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
            positionH.write(math::vec3(10, 2.0f, 10.0f));
            scaleH.write(math::vec3(5.0f, 1.0f,5.0f));

            auto rotation = rotationH.read();

            ent.write_component(rotation);

        }

    }

    void PhysicsFractureTestSystem::explosionTest()
    {
        CreateElongatedFloor(math::vec3(20.0, 1.0f, 9.8f), math::quat(), math::vec3(20, 1, 20));

        smallExplosionEnt = CreateSplitTestBox(physics::cube_collider_params(1.0f,1.0f,1.0f), math::vec3(12.0, 2.0f, 9.8f),
            math::quat(), textureH, true, false, math::vec3());

        mediumExplosionEnt = CreateSplitTestBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f), math::vec3(20.0, 2.0f, 9.8f),
            math::quat(), textureH, true, false, math::vec3());

        largeExplosionEnt = CreateSplitTestBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f), math::vec3(28.0, 2.0f, 9.8f),
            math::quat(), textureH, true, false, math::vec3());

    }

    void PhysicsFractureTestSystem::spawnEntityOnCameraForward(spawnEntity* action)
    {
        if (!action->value)
        {
            auto cameraQuery = createQuery<rendering::camera>();
            cameraQuery.queryEntities();

            for (auto ent : cameraQuery)
            {
                auto [position, rotation, scale] = ent.get_component_handles<transform>();
                math::mat4 trans = math::compose(scale.read(), rotation.read(), position.read());
                math::vec3 dir = math::normalize(trans * math::vec4(0, 0, 1, 0));

                dir *= 10;

                CreateSplitTestBox(physics::cube_collider_params(1, 1, 1), trans[3],
                    math::quat(), textureH, false, true, dir);

            }

        }
       

    }

    void PhysicsFractureTestSystem::simpleMinecraftHouse()
    {
        //Floor

        std::vector<rendering::material_handle> woodInitialThenStone{ woodTextureH,  rockTextureH,  rockTextureH ,  rockTextureH ,woodTextureH };
        std::vector<rendering::material_handle>Stone{ rockTextureH };
        std::vector<rendering::material_handle>Wood{ woodTextureH };

        std::vector<int> initialOnly{ 1,2,3 };
        std::vector<int> window{ 2 };
        std::vector<int> ignoreEmpty;

        float level = 2.0f;
        createFloor(1, 5, math::vec3(0.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, woodInitialThenStone, ignoreEmpty);
        createFloor(1, 5, math::vec3(1.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, ignoreEmpty);
        createFloor(1, 5, math::vec3(2.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, ignoreEmpty);
        createFloor(1, 5, math::vec3(3.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, ignoreEmpty);
        createFloor(1, 5, math::vec3(4.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, woodInitialThenStone, ignoreEmpty);
       
        level += 1.0f;
        //first stack
        createFloor(1, 5, math::vec3(0.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, woodInitialThenStone, ignoreEmpty);
        createFloor(1, 5, math::vec3(1.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, initialOnly);
        createFloor(1, 1, math::vec3(2.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, ignoreEmpty);
        createFloor(1, 5, math::vec3(3.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, initialOnly);
        createFloor(1, 5, math::vec3(4.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, woodInitialThenStone, ignoreEmpty);

        //window stack
        level += 1.0f;
        createFloor(1, 5, math::vec3(0.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, woodInitialThenStone, window);
        createFloor(1, 5, math::vec3(1.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, initialOnly);
        createFloor(1, 1, math::vec3(2.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, ignoreEmpty);
        createFloor(1, 5, math::vec3(3.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, initialOnly);
        createFloor(1, 5, math::vec3(4.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, woodInitialThenStone, window);

        level += 1.0f;
        //third stack
        createFloor(1, 5, math::vec3(0.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, woodInitialThenStone, ignoreEmpty);
        createFloor(1, 5, math::vec3(1.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, initialOnly);
        createFloor(1, 5, math::vec3(2.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, initialOnly);
        createFloor(1, 5, math::vec3(3.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Stone, initialOnly);
        createFloor(1, 5, math::vec3(4.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, woodInitialThenStone, ignoreEmpty);

        level += 1.0f;
        createFloor(1, 5, math::vec3(0.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Wood, ignoreEmpty);
        createFloor(1, 5, math::vec3(1.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Wood, ignoreEmpty);
        createFloor(1, 5, math::vec3(2.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Wood, ignoreEmpty);
        createFloor(1, 5, math::vec3(3.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Wood, ignoreEmpty);
        createFloor(1, 5, math::vec3(4.0f, level, 10.0f),
            math::vec3(1.0f), cubeH, Wood, ignoreEmpty);

        PhysicsSystem::IsPaused = false;
        CreateElongatedFloor(math::vec3(5.0, 1.0f, 9.8f), math::quat(), math::vec3(20, 1, 20));

    }

    void PhysicsFractureTestSystem::numericalRobustnessTest()
    {
        physics::cube_collider_params cubeParams;
        cubeParams.breadth = 1.0f;
        cubeParams.width = 1.0f;
        cubeParams.height = 1.0f;


        //Position Test
        ecs::entity_handle wall;
        {
            wall = m_ecs->createEntity();
            auto entPhyHande = wall.add_component<physics::physicsComponent>();
            wall.add_component<rigidbody>();
            //wall.add_component<physics::Fracturer>();

            physics::physicsComponent physicsComponent2;


            physicsComponent2.AddBox(cubeParams);

            entPhyHande.write(physicsComponent2);

            wall.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(textureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(wall);
            positionH.write(math::vec3(0, 12.0f, 10.0f));
            scaleH.write(math::vec3(1.0f, 1.0f, 1.0f));

            auto rotation = rotationH.read();
            rotation *= math::angleAxis(math::deg2rad(30.0f), math::vec3(1, 0, 0));
            rotation *= math::angleAxis(math::deg2rad(50.0f), math::vec3(0, 1, 0));
            wall.write_component(rotation);

            auto splitterH = wall.add_component<physics::MeshSplitter>();
            auto splitter = splitterH.read();
            splitter.InitializePolygons(wall);
            splitterH.write(splitter);


            auto idH = wall.add_component<physics::identifier>();
            auto id = idH.read();
            id.id = "WALL";
            idH.write(id);
        }

        ecs::entity_handle wall2;
        {
            wall2 = m_ecs->createEntity();


            auto entPhyHande = wall2.add_component<physics::physicsComponent>();
            wall2.add_component<rigidbody>();
            //wall2.add_component<physics::Fracturer>();

            physics::physicsComponent physicsComponent2;


            physicsComponent2.AddBox(cubeParams);

            entPhyHande.write(physicsComponent2);

            wall2.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(textureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(wall2);
            positionH.write(math::vec3(50.0f, 12.0f, 10.0f));
            scaleH.write(math::vec3(1.0f, 1.0f, 1.0f));

            auto rotation = rotationH.read();
            rotation *= math::angleAxis(math::deg2rad(30.0f), math::vec3(1, 0, 0));
            rotation *= math::angleAxis(math::deg2rad(50.0f), math::vec3(0, 1, 0));
            wall2.write_component(rotation);

            auto splitterH = wall2.add_component<physics::MeshSplitter>();
            auto splitter = splitterH.read();
            splitter.InitializePolygons(wall2);
            splitterH.write(splitter);


            //auto idH = wall2.add_component<physics::identifier>();
            //auto id = idH.read();
            //id.id = "WALL";
            //idH.write(id);
        }

        ecs::entity_handle wall3;
        {
            wall3 = m_ecs->createEntity();


            auto entPhyHande = wall3.add_component<physics::physicsComponent>();
            wall3.add_component<rigidbody>();
            //wall2.add_component<physics::Fracturer>();

            physics::physicsComponent physicsComponent2;


            physicsComponent2.AddBox(cubeParams);

            entPhyHande.write(physicsComponent2);

            wall3.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(textureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(wall3);
            positionH.write(math::vec3(200.0f, 12.0f, 10.0f));
            scaleH.write(math::vec3(1.0f, 1.0f, 1.0f));

            auto rotation = rotationH.read();
            rotation *= math::angleAxis(math::deg2rad(30.0f), math::vec3(1, 0, 0));
            rotation *= math::angleAxis(math::deg2rad(50.0f), math::vec3(0, 1, 0));
            wall3.write_component(rotation);

            auto splitterH = wall3.add_component<physics::MeshSplitter>();
            auto splitter = splitterH.read();
            splitter.InitializePolygons(wall2);
            splitterH.write(splitter);


            //auto idH = wall2.add_component<physics::identifier>();
            //auto id = idH.read();
            //id.id = "WALL";
            //idH.write(id);
        }

        CreateElongatedFloor(math::vec3(0.0f, 1.0f, 10.0f), math::quat(), math::vec3(5.0f, 1.0f, 5.0f));

        rotation rot = math::angleAxis(math::deg2rad(90.0f), math::vec3(1, 0, 0));
        CreateElongatedFloor(math::vec3(0.0f, 2.0f, 7.0f), rot, math::vec3(5.0f, 1.0f, 5.0f));

        ////////////////////////////////////////////////
        CreateElongatedFloor(math::vec3(50.0f, 1.0f, 10.0f), math::quat(), math::vec3(5.0f, 1.0f, 5.0f));
        CreateElongatedFloor(math::vec3(50.0f, 2.0f, 7.0f), rot, math::vec3(5.0f, 1.0f, 5.0f));

        /////////////////////////////////////////////////////
        CreateElongatedFloor(math::vec3(200.0f, 1.0f, 10.0f), math::quat(), math::vec3(5.0f, 1.0f, 5.0f));
        CreateElongatedFloor(math::vec3(200.0f, 2.0f, 7.0f), rot, math::vec3(5.0f, 1.0f, 5.0f));


        CreateElongatedFloor(math::vec3(-50.0f, 0.0f, -50.0f), math::quat(), math::vec3(90.0f, 5.0f, 90.0f));

        CreateSplitTestBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f), math::vec3(-50.0f, 15.0f, -50.0f),
            math::quat(),textureH, false);

        CreateSplitTestBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f), math::vec3(-35.0f, 15.0f, -35.0f),
            math::quat(), textureH, false);

        CreateSplitTestBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f), math::vec3(-15.0f, 15.0f, -15.0f),
            math::quat(), textureH, false);
    }

    void PhysicsFractureTestSystem::fractureTest()
    {
        physics::cube_collider_params cubeParams;
        cubeParams.breadth = 1.0f;
        cubeParams.width = 1.0f;
        cubeParams.height = 1.0f;


        physics::cube_collider_params scaledCubeParams(-5.0f,5.0f,1.0f);
        //-------------------------------------------------------------------------------------------------------------------------------//
                                                       //WALL
        //-------------------------------------------------------------------------------------------------------------------------------//

        /*CreateSplitTestBox(cubeParams, math::vec3(0.0f, 2.0f, 9.8f),
            math::quat(), true);

        CreateSplitTestBox(cubeParams, math::vec3(0.0f, 3.0f, 9.8f),
            math::quat(), true);*/

 

        //-------------------------------------------------------------------------------------------------------------------------------//
                                                     //THROWN OBJECT TEST 
        //-------------------------------------------------------------------------------------------------------------------------------//

        ecs::entity_handle block;
        {
            block = m_ecs->createEntity();

            auto entPhyHande = block.add_component<physics::physicsComponent>();
            auto rbH = block.add_component<physics::rigidbody>();

            auto rb = rbH.read();
            rb.velocity = math::vec3(6, 0, 0);
            rb.setMass(3.0f);
            rbH.write(rb);

            physics::physicsComponent physicsComponent2;
            

            physicsComponent2.AddBox(cubeParams);

            entPhyHande.write(physicsComponent2);

            block.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(textureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(block);
            positionH.write(math::vec3(-3.0f, 3.0f, 9.8f));
            scaleH.write(math::vec3(1.0f, 1.0f, 1.0f));

            auto rotation = rotationH.read();
            rotation *= math::angleAxis(math::deg2rad(45.0f), math::vec3(0, 1, 0));
            rotation *= math::angleAxis(math::deg2rad(-60.0f), math::vec3(1, 0, 0));
            block.write_component(rotation);

           

        }

        CreateElongatedFloor(math::vec3(0, 1.0f, 9.8f), math::quat(), math::vec3(5,1,5));
    }

    void PhysicsFractureTestSystem::CreateElongatedFloor(math::vec3 position, math::quat rot, math::vec3 scale)
    {
        cube_collider_params scaledCubeParams(scale.x,scale.z,scale.y);
        ecs::entity_handle floor5;
        {
            floor5 = m_ecs->createEntity();

            auto entPhyHande = floor5.add_component<physics::physicsComponent>();

            physics::physicsComponent physicsComponent2;
            physicsComponent2.AddBox(scaledCubeParams);
            entPhyHande.write(physicsComponent2);

            //floor.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(woodTextureH));

            auto idH = floor5.add_component<physics::identifier>();
            auto id = idH.read();
            id.id = "floor";
            idH.write(id);


            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(floor5);
            positionH.write(position);
            rotationH.write(rot);
            scaleH.write(math::vec3(1.0f, 1.0f, 1.0f));
        }

        ecs::entity_handle floor6;
        {
            floor6 = m_ecs->createEntity();

            floor6.add_components<rendering::mesh_renderable>
                (mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(woodTextureH));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(floor6);
            positionH.write(position);
            rotationH.write(rot);
            scaleH.write(scale);

        }
    }

    void PhysicsFractureTestSystem::OnSplit(physics_split_test* action)
    {
        static ecs::EntityQuery halfEdgeQuery = createQuery<physics::MeshSplitter>();

        if (action->value)
        {
            halfEdgeQuery.queryEntities();

            for (auto entity : halfEdgeQuery)
            {
                auto edgeFinderH = entity.get_component_handle<physics::MeshSplitter>();
                auto edgeFinder = edgeFinderH.read();
                edgeFinder.TestSplit();
                edgeFinderH.write(edgeFinder);
                edgeFinder.DestroyTestSplitter(m_ecs);

            }
        }
    }

    ecs::entity_handle PhysicsFractureTestSystem::CreateSplitTestBox(physics::cube_collider_params cubeParams,math::vec3 position,
        math::quat rotation, rendering::material_handle mat, bool isFracturable,bool hasRigidbody ,math::vec3 velocity)
    {
        auto wall = m_ecs->createEntity();
        auto entPhyHande = wall.add_component<physics::physicsComponent>();

        if (hasRigidbody)
        {
           auto rbH = wall.add_component<rigidbody>();
           auto rb = rbH.read();
           rb.velocity = velocity;
           rbH.write(rb);
        }

        if (isFracturable)
        {
            wall.add_component<physics::Fracturer>();
        }


        physics::physicsComponent physicsComponent2;


        physicsComponent2.AddBox(cubeParams);

        entPhyHande.write(physicsComponent2);

        wall.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(mat));

        auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(wall);
        positionH.write(position);
        scaleH.write(math::vec3(1.0f, 1.0f, 1.0f));
        rotationH.write(rotation);

        if (isFracturable)
        {
            auto splitterH = wall.add_component<physics::MeshSplitter>();
            auto splitter = splitterH.read();
            splitter.InitializePolygons(wall);
            splitterH.write(splitter);
        }

        return  wall;
    }

    void PhysicsFractureTestSystem::createFloor(int xCount, int yCount, math::vec3 start,
        math::vec3 offset, rendering::model_handle cubeH,std::vector< rendering::material_handle> materials,std::vector<int> ignoreJ)
    {
        for (size_t i = 0; i < xCount; i++)
        {
            for (size_t j = 0; j < yCount; j++)
            {
                bool shouldSkip = false;
                for (auto ignoreCount : ignoreJ)
                {
                    if (j == ignoreCount)
                    {
                        shouldSkip = true;
                    }
                }

                if (shouldSkip) { continue; }

                auto material_handle = materials.at(  j%materials.size());
                CreateSplitTestBox(physics::cube_collider_params(1.0f, 1.0f, 1.0f), start
                    + math::vec3(offset.x * i, 0, offset.z * j),
                    math::quat(), material_handle, true);
            }
        }
    }

    void PhysicsFractureTestSystem::smallExplosionTest(smallExplosion* action)
    {
        if (!action->value)
        {
            *reinterpret_cast<int*>(&m_boom) |= SMALL_BOOM;
        }

    }

    void PhysicsFractureTestSystem::mediumExplosionTest(mediumExplosion* action)
    {
        if (!action->value)
        {
            *reinterpret_cast<int*>(&m_boom) |= MEDIUM_BOOM;
        }
       
    }

    void PhysicsFractureTestSystem::largeExplosionTest(largeExplosion* action)
    {
        if (!action->value)
        {
            *reinterpret_cast<int*>(&m_boom) |= BIG_BOOM;
        }
    
    }

    void PhysicsFractureTestSystem::explodeAThing(time::span)
    {
        if (m_boom & SMALL_BOOM)
        {
            log::debug("smallExplosionTest");
            auto PosH = smallExplosionEnt.get_component_handle<position>();
            auto fracturerH = smallExplosionEnt.get_component_handle<Fracturer>();
            auto fracturer = fracturerH.read();
            FractureParams param(PosH.read(), 10.0f);
            fracturer.ExplodeEntity(smallExplosionEnt, param);
            fracturerH.write(fracturer);

        }

        if (m_boom & MEDIUM_BOOM)
        {
            auto PosH = mediumExplosionEnt.get_component_handle<position>();
            auto fracturerH = mediumExplosionEnt.get_component_handle<Fracturer>();
            auto fracturer = fracturerH.read();
            FractureParams param(PosH.read(), 50.0f);
            fracturer.ExplodeEntity(mediumExplosionEnt, param);
            fracturerH.write(fracturer);
        }

        if (m_boom & BIG_BOOM)
        {
            auto PosH = largeExplosionEnt.get_component_handle<position>();
            auto fracturerH = largeExplosionEnt.get_component_handle<Fracturer>();
            auto fracturer = fracturerH.read();
            FractureParams param(PosH.read(), 200.0f);
            fracturer.ExplodeEntity(largeExplosionEnt, param);
            fracturerH.write(fracturer);
        }

        m_boom = NO_BOOM;
    }
    

}


