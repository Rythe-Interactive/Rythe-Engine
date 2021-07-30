#include <physics/systems/physics_fracture_test_system.hpp>
#include <physics/mesh_splitter_utils/mesh_splitter.hpp>
#include <physics/mesh_splitter_utils/splittable_polygon.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/systems/physicssystem.hpp>
#include <physics/components/fracturer.hpp>
#include <rendering/debugrendering.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/rendering.hpp>
#include <physics/components/fracturecountdown.hpp>

namespace legion::physics
{
    void PhysicsTestSystem::setup()
    {
        using namespace legion::core::fs::literals;

        physics::PrimitiveMesh::SetECSRegistry(m_ecs);

        auto win = world.read_component<app::window>();

        app::context_guard guard(win);

        #pragma region Material Setup
   
        auto litShader = rendering::ShaderCache::create_shader("lit", fs::view("engine://shaders/default_lit.shs"));
        vertexColor = rendering::MaterialCache::create_material("vertexColor", "assets://shaders/vertexcolor.shs"_view);
        textureH = rendering::MaterialCache::create_material("texture", "assets://shaders/texture.shs"_view);
        textureH.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/split-test.png"_view));

        /////////////////
        woodTextureH = rendering::MaterialCache::create_material("texture2", "assets://shaders/texture.shs"_view);
        woodTextureH.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/test-albedo.png"_view));

        rockTextureH = rendering::MaterialCache::create_material("rock", "assets://shaders/texture.shs"_view);
        rockTextureH.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/rock.png"_view));

        concreteH = rendering::MaterialCache::create_material("concrete", "assets://shaders/texture.shs"_view);
        concreteH.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/concrete.png"_view));

        brickH = rendering::MaterialCache::create_material("brick", "assets://shaders/texture.shs"_view);
        brickH.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/plaster/plasterColor.png"_view));

        //auto litShader = rendering::ShaderCache::create_shader("lit", fs::view("engine://shaders/default_lit.shs"));
       /* tileH = rendering::MaterialCache::create_material("tile", "assets://shaders/texture.shs"_view);
        tileH.set_param("_texture", rendering::TextureCache::create_texture("assets://textures/tile/tileColor.png"_view)); */

        //log::debug("------------------------------ TILE -------------");
 
        tileH = rendering::MaterialCache::create_material("tile", litShader);
        tileH.set_param("alphaCutoff", 0.5f);
        tileH.set_param("useAlbedoTex", true);
        tileH.set_param("useRoughnessTex", true);
        tileH.set_param("useNormal", true);

        tileH.set_param("useEmissiveTex", false);
        tileH.set_param("useAmbientOcclusion", false);
        tileH.set_param("useHeight", false);
        tileH.set_param("useMetallicTex", false);
        tileH.set_param("useMetallicRoughness", false);

        tileH.set_param("metallicValue", 0.0f);
        tileH.set_param("emissiveColor", math::colors::black);

        tileH.set_param("albedoTex", rendering::TextureCache::create_texture("assets://textures/tile/tileColor.png"_view));
        tileH.set_param("normalTex", rendering::TextureCache::create_texture("assets://textures/tile/tileNormal.png"_view));
        tileH.set_param("roughnessTex", rendering::TextureCache::create_texture("assets://textures/tile/tileRoughness.png"_view));
        tileH.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));

        wireFrameH = rendering::MaterialCache::create_material("wireframe", "assets://shaders/wireframe.shs"_view);
        #pragma endregion

        #pragma region Model Setup
        directionalLightH = rendering::ModelCache::create_model("directional light", "assets://models/directional-light.obj"_view);
        cubeH = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
        planeH = rendering::ModelCache::create_model("plane", "assets://models/plane.obj"_view);
        cylinderH = rendering::ModelCache::create_model("cylinder", "assets://models/cylinder.obj"_view);
        concaveTestObject = rendering::ModelCache::create_model("concaveTestObject", "assets://models/polygonTest.obj"_view);
        colaH = rendering::ModelCache::create_model("cola", "assets://models/cola.glb"_view);
        hammerH = rendering::ModelCache::create_model("hammer", "assets://models/hammer.obj"_view);
        suzzaneH = rendering::ModelCache::create_model("suzanne", "assets://models/suzanne.glb"_view);
        teapotH = rendering::ModelCache::create_model("sah", "assets://models/gnomecentered.obj"_view);

        #pragma endregion

        #pragma region Input binding

        app::InputSystem::createBinding<extendedPhysicsContinue>(app::inputmap::method::M);
        app::InputSystem::createBinding<nextPhysicsTimeStepContinue>(app::inputmap::method::N);
        //app::InputSystem::createBinding<spawnEntity>(app::inputmap::method::MOUSE_LEFT);
        app::InputSystem::createBinding<QHULL>(app::inputmap::method::Q);
        app::InputSystem::createBinding<AddRigidbody>(app::inputmap::method::R);
        app::InputSystem::createBinding<SpawnRandomHullOnCameraLoc>(app::inputmap::method::F);
        app::InputSystem::createBinding<SpawnHullActive>(app::inputmap::method::P);

        #pragma endregion

        #pragma region Function binding
        bindToEvent<extendedPhysicsContinue, &PhysicsTestSystem::extendedContinuePhysics>();
        bindToEvent<nextPhysicsTimeStepContinue, &PhysicsTestSystem::OneTimeContinuePhysics>();

        bindToEvent<QHULL, &PhysicsTestSystem::quickHullStep>();
        bindToEvent<AddRigidbody, &PhysicsTestSystem::AddRigidbodyToQuickhulls>();
        bindToEvent< SpawnRandomHullOnCameraLoc, &PhysicsTestSystem::spawnRandomConvexHullOnCameraLocation>();
        bindToEvent< SpawnHullActive, &PhysicsTestSystem::ActivateSpawnRandomHull>();

        #pragma endregion

        auto lightshader = rendering::ShaderCache::create_shader("light", "assets://shaders/light.shs"_view);
        directionalLightMH = rendering::MaterialCache::create_material("directional light", lightshader);
        directionalLightMH.set_param("color", math::color(1, 1, 0.8f));
        directionalLightMH.set_param("intensity", 1.f);

        {
            auto sun = createEntity();
            sun.add_components<rendering::mesh_renderable>(mesh_filter(directionalLightH.get_mesh()), rendering::mesh_renderer(directionalLightMH));
            sun.add_component<rendering::light>(rendering::light::directional(math::color(1, 1, 0.8f), 10.f));
            sun.add_components<transform>(position(10, 10, 10), rotation::lookat(math::vec3(1, 1, -1), math::vec3::zero), scale());
        }


        createProcess<&PhysicsTestSystem::colliderDraw>("Physics",0.02f);

        quickhullTestScene();
        //BoxStackScene();
        //stabilityComparisonScene();
        //monkeyStackScene();

        Fracturer::registry = m_ecs;
    }

    void PhysicsTestSystem::colliderDraw(time::span dt)
    {
        //drawPhysicsColliders();

        auto query = createQuery<ObjectToFollow>();
        query.queryEntities();

        for (auto ent : query)
        {
            auto objToFollow = ent.read_component<ObjectToFollow>();

            auto [posH,rotH,scaleH] = ent.get_component_handles<transform>();
            auto [posH2, rotH2, scaleH2] = objToFollow.ent.get_component_handles<transform>();

            posH.write(posH2.read());
            rotH.write(rotH2.read());
            scaleH.write(scaleH2.read());
        }


        auto cameraQuery = createQuery<rendering::camera,transform>();
        cameraQuery.queryEntities();
        for (auto ent : cameraQuery)
        {
            auto [posH, rotH, scaleH] = ent.get_component_handles<transform>();
            //core::position newPos = posH.read() + math::vec3(0.05f, 0, 0);
            //posH.write(newPos);
        }


    }

    void PhysicsTestSystem::ActivateSpawnRandomHull(SpawnHullActive* action)
    {
        if (!action->value)
        {
            m_throwingHullActivated = !m_throwingHullActivated;
            log::debug("Spawn Hull Active: {0}", m_throwingHullActivated);
        }
    }

    void PhysicsTestSystem::spawnRandomConvexHullOnCameraLocation(SpawnRandomHullOnCameraLoc* action)
    {
        if (!m_throwingHullActivated || action->value) { return; }

        //log::debug("spawnRandomConvexHullOnCamerLocation");

        //create entity
        auto ent = m_ecs->createEntity();

        //add a transform component
        auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);

        //get camera position and set transform to camera postiion 
        auto cameraQuery = createQuery<rendering::camera, transform>();
        cameraQuery.queryEntities();

        math::vec3 cameraDirection;

        for (auto ent : cameraQuery)
        {
            auto [positionCamH, rotationCamH, scaleCamH] = ent.get_component_handles<transform>();
            cameraDirection = rotationCamH.read() * math::vec3(0, 0, 1);
            positionH.write(positionCamH.read() + cameraDirection * 2.5f);
            
        }

        //randomly generated a number of vertices
        std::vector<math::vec3> quickhullVertices;

        {
            const math::vec3 right = math::vec3(1.0f, 0.0f, 0.0f);
            const math::vec3 up = math::vec3(0.0f, 1.0f, 0.0f);
            const math::vec3 forward = math::vec3(0.0f, 0.0f, 1.0f);

            static std::mt19937 generator;
            std::uniform_real_distribution<double> dis(-1.0f, 1.0f);

            for (size_t i = 0; i < 20; i++)
            {
                math::vec3 rightVal = dis(generator) * right;
                math::vec3 upVal = dis(generator) * up;
                math::vec3 forwardVal = dis(generator) * forward;

                quickhullVertices.push_back(rightVal + upVal + forwardVal);

            }
        
        }

        {
            //shift vertices based on centroid
            math::vec3 centroid(1.0f);

            for (auto& vert : quickhullVertices)
            {
                centroid += vert;
                //debug::drawLine(positionH.read(), positionH.read() + vert, math::colors::red, 5.0f, FLT_MAX, false);
            }

            centroid /= quickhullVertices.size();
            //debug::drawLine(positionH.read(), positionH.read() + centroid, math::colors::blue, 5.0f, FLT_MAX, false);

            for (auto& vert : quickhullVertices)
            {
                vert -= centroid;
            }
        }
      

        //add a rigidbody
        auto rbH = ent.add_component<physics::rigidbody>();
        auto rb = rbH.read();

        rb.setMass(2.5f);
        rb.localInverseInertiaTensor = math::mat3(3.0f);
        rb.velocity = cameraDirection * 14.0f;

        rbH.write(rb);

        //add a physics component and run quickhull
        physics::physicsComponent physicsComponent;
        auto entPhyHande = ent.add_component<physics::physicsComponent>();
        physicsComponent.ConstructConvexHullFromVertices(quickhullVertices);
        entPhyHande.write(physicsComponent);

        //using vertices of convex hull, create a rendering mesh out of it
        auto convexCollider = std::dynamic_pointer_cast<ConvexCollider>(physicsComponent.colliders.at(0));
        mesh newMesh;

        std::vector<uint>& indices = newMesh.indices;
        std::vector<math::vec3>& vertices = newMesh.vertices;
        std::vector<math::vec2>& uvs = newMesh.uvs;
        std::vector<math::vec3>& normals = newMesh.normals;
        int x = 0;
        for (auto face : convexCollider->GetHalfEdgeFaces())
        {
            math::vec3 faceCentroid = face->centroid;

            math::vec3 faceForward = math::normalize(face->startEdge->edgePosition - faceCentroid);
            math::vec3 faceRight = math::cross(face->normal, faceForward);

            std::vector<math::vec3> faceVertices;
            auto collectVertices = [&faceVertices](HalfEdgeEdge* edge) {faceVertices.push_back(edge->edgePosition ); };
            face->forEachEdge(collectVertices);

            math::vec3 maxForward, minForward, maxRight, minRight;

            //get support point for the tangents and inverse tangents of this face 
            PhysicsStatics::GetSupportPoint(faceVertices, faceForward, maxForward);
            PhysicsStatics::GetSupportPoint(faceVertices, -faceRight, minForward);

            PhysicsStatics::GetSupportPoint(faceVertices, faceRight, maxRight);
            PhysicsStatics::GetSupportPoint(faceVertices, -faceRight, minRight);

            float maxForwardLength = math::dot(maxForward - faceCentroid, faceForward);
            float minForwardLength = math::dot(minForward - faceCentroid, -faceForward);

            float maxRightLength = math::dot(maxRight - faceCentroid, faceRight);
            float minRightLength = math::dot(minRight - faceCentroid, -faceRight);

            math::vec3 min = faceCentroid - (faceForward * minForwardLength) - (faceRight * minRightLength);
            float forwardLength = (minForwardLength + maxForwardLength);
            float rightLength =  (minRightLength + maxRightLength);

            auto calculateUV = [&min,&faceForward,&faceRight,&forwardLength,&rightLength](math::vec3 edgePosition)->math::vec2
            {
                math::vec2 result;
                result.x = math::dot(edgePosition - min, faceForward)/ forwardLength;
                result.y = math::dot(edgePosition - min, faceRight)/ rightLength;

                return result;
            };

            auto populateMesh = [&calculateUV,&vertices,&normals,&uvs](HalfEdgeEdge* edge)
            {
                math::vec3 normal = edge->face->normal;
               
                vertices.push_back(edge->edgePosition);
                uvs.push_back(calculateUV(edge->edgePosition));
                normals.push_back(normal);

                //log::debug("")

                vertices.push_back(edge->nextEdge->edgePosition);
                uvs.push_back(calculateUV(edge->nextEdge->edgePosition));
                normals.push_back(normal);

                vertices.push_back(edge->face->centroid);
                uvs.push_back(calculateUV(edge->face->centroid));
                normals.push_back(normal);

            };

            face->forEachEdge(populateMesh);

        }


        for (int i = 0; i < vertices.size(); i++)
        {
            indices.push_back(i);
        }

        sub_mesh newSubMesh;
        newSubMesh.indexCount = newMesh.indices.size();
        newSubMesh.indexOffset = 0;

        newMesh.submeshes.push_back(newSubMesh);

        static int count = 0;
        mesh_handle meshH = core::MeshCache::create_mesh("newMesh" + std::to_string(count), newMesh);
        auto modelH = rendering::ModelCache::create_model(meshH);
        count++;

        //create renderable
        mesh_filter meshFilter = mesh_filter(meshH);

        ent.add_components<rendering::mesh_renderable>(meshFilter, rendering::mesh_renderer(concreteH));
        //using extents of face, define uvs

        //randomly select texture

        //set rendering mesh

    }

    rendering::material_handle defaultStairMaterial;

    void PhysicsTestSystem::quickhullTestScene()
    {
        math::mat3 elongatedBlockInertia = math::mat3(math::vec3(6.0f, 0, 0), math::vec3(0.0f, 3.0f, 0), math::vec3(0, 0, 6.0f));

        //cube
        createQuickhullTestObject
        (math::vec3(0,5.0f, -0.8f),cubeH, wireFrameH);

        //cup
        createQuickhullTestObject
        (math::vec3(5.0f, 5.0f, -0.8f), colaH, wireFrameH, elongatedBlockInertia);

        //////hammer
        createQuickhullTestObject
        (math::vec3(10.0f, 5.0f, -0.8f), hammerH, wireFrameH);

        ////suzanne
        createQuickhullTestObject
        (math::vec3(15.0f, 5.0f, -0.8f), suzzaneH, wireFrameH);

        ////ohio teapot
        createQuickhullTestObject
        (math::vec3(20.0f, 5.0f, -0.5f), teapotH, wireFrameH,elongatedBlockInertia);

        
        defaultStairMaterial = textureH;
        addStaircase(math::vec3(8, 2, 0));

        defaultStairMaterial = tileH;
        addStaircase(math::vec3(8, 1, -1));

        defaultStairMaterial = textureH;
        addStaircase(math::vec3(8, 0, -2));

        defaultStairMaterial = tileH;
        addStaircase(math::vec3(8, -1, -3.1f));

        defaultStairMaterial = textureH;
        addStaircase(math::vec3(8, -2, -5),5.0f);

        for (size_t i = 0; i < registeredColliderColorDraw.size(); i++)
        {
            folowerObjects.push_back(std::vector<ecs::entity_handle>());
        }
    }

    

    void PhysicsTestSystem::BoxStackScene()
    {
        cube_collider_params cubeParams;
        cubeParams.breadth = 1.0f;
        cubeParams.height = 1.0f;
        cubeParams.width = 1.0f;
        
        createStack(4, 4, 4, math::vec3(3.5f,0,3.5f), math::vec3(1.0f),
            cubeH, textureH, cubeParams,false);

        defaultStairMaterial = tileH;
        addStaircase(math::vec3(5.0f, -1, 5.0f), 10.0f, 10.0f);
    }

    void PhysicsTestSystem::stabilityComparisonScene()
    {
        cube_collider_params cubeParams;
        cubeParams.breadth = 1.0f;
        cubeParams.height = 1.0f;
        cubeParams.width = 1.0f;

        createStack(1,1,10, math::vec3(0.0f, 0, 0.0f), math::vec3(1.0f),
            cubeH, textureH, cubeParams,true,1.0f);

        defaultStairMaterial = tileH;
        addStaircase(math::vec3(0.0f, -1, 0.0f), 4.0f, 4.0f);

    }

    void PhysicsTestSystem::monkeyStackScene()
    {
        cube_collider_params cubeParams;
        cubeParams.breadth = 1.0f;
        cubeParams.height = 1.0f;
        cubeParams.width = 1.0f;

        createStack(3, 3, 1, math::vec3(3.5f, 0.5f, 3.5f), math::vec3(2.0f),
            suzzaneH, tileH, cubeParams, true);

        defaultStairMaterial = textureH;
        addStaircase(math::vec3(5.0f, -1, 5.0f), 10.0f, 10.0f);
    }

    

    void PhysicsTestSystem::addStaircase(math::vec3 position, float breadthMult, float widthMult )
    {
        physics::cube_collider_params cubeParams;
        cubeParams.breadth = breadthMult;
        cubeParams.width = widthMult;
        cubeParams.height = 1.0f;

        auto ent = m_ecs->createEntity();

        auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
        positionH.write(position);

        //ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(textureH));

        auto entPhyHande = ent.add_component<physics::physicsComponent>();
        physics::physicsComponent physicsComponent2;
        physicsComponent2.AddBox(cubeParams);
        entPhyHande.write(physicsComponent2);

        auto ent2 = m_ecs->createEntity();
        ent2.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(defaultStairMaterial));

        auto [position2H, rotation2H, scale2H] = m_ecs->createComponents<transform>(ent2);
        position2H.write(position);
        scale2H.write(math::vec3(cubeParams.width, 1.0f, breadthMult));
    }

    void PhysicsTestSystem::createQuickhullTestObject(math::vec3 position, rendering::model_handle cubeH, rendering::material_handle TextureH, math::mat3 inertia )
    {
        physics::cube_collider_params cubeParams;
        cubeParams.breadth = 1.0f;
        cubeParams.width = 1.0f;
        cubeParams.height = 1.0f;

        auto ent = m_ecs->createEntity();

        auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
        positionH.write(position);

        ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(TextureH));

        auto entPhyHande = ent.add_component<physics::physicsComponent>();

        auto rbH = ent.add_component<rigidbody>();
        auto rb = rbH.read();
        rb.localInverseInertiaTensor = inertia;
        rbH.write(rb);

        registeredColliderColorDraw.push_back(ent);
    }

    void PhysicsTestSystem::PopulateFollowerList(ecs::entity_handle physicsEnt, int index)
    {
        app::window window = m_ecs->world.get_component_handle<app::window>().read();

        auto physicsComp = physicsEnt.read_component<physicsComponent>();
        auto collider = std::dynamic_pointer_cast<ConvexCollider>(physicsComp.colliders.at(0));
        auto [posH, rotH, scaleH] = physicsEnt.get_component_handles<transform>();

        auto& currentContainer = folowerObjects.at(index);

        for (auto ent : currentContainer)
        {
            m_ecs->destroyEntity(ent);
        }

        folowerObjects.at(index).clear();

        for (auto face : collider->GetHalfEdgeFaces())
        {
            //populate localVert
            math::vec3 localCentroid = face->centroid;
            std::vector<math::vec3> localVert;

            auto populateVectorLambda = [&localVert](physics::HalfEdgeEdge* edge)
            {
                localVert.push_back(edge->edgePosition);
            };

            face->forEachEdge(populateVectorLambda);

            //initialize mesh
            legion::core::mesh newMesh;

            std::vector<math::vec3> vertices;
            std::vector<uint> indices;
            std::vector<math::vec3> normals;
            std::vector<math::vec2> uvs;

            for (size_t i = 0; i < localVert.size(); i++)
            {
                vertices.push_back(localVert.at(i));
                vertices.push_back(localVert.at((i + 1) % localVert.size()));
                vertices.push_back(localCentroid);
            }

            for (size_t i = 0; i < localVert.size(); i++)
            {
                normals.push_back(math::vec3(face->normal));
            }

            for (size_t i = 0; i < vertices.size(); i++)
            {
                indices.push_back(i);
            }

            for (size_t i = 0; i < vertices.size(); i++)
            {
                uvs.push_back(math::vec2());
            }


            newMesh.vertices = vertices;
            newMesh.indices = indices;
            newMesh.uvs = uvs;
            newMesh.normals = normals;

            newMesh.calculate_tangents(&newMesh);

            sub_mesh newSubMesh;
            newSubMesh.indexCount = newMesh.indices.size();
            newSubMesh.indexOffset = 0;

            newMesh.submeshes.push_back(newSubMesh);

            //creaate modelH
            static int count = 0;
            mesh_handle meshH = core::MeshCache::create_mesh("newMesh" + std::to_string(count), newMesh);
            auto modelH = rendering::ModelCache::create_model(meshH);


            auto newEnt = m_ecs->createEntity();

            rendering::material_handle newMat;
            {
                app::context_guard guard(window);
                auto colorShader = rendering::ShaderCache::create_shader("color" + std::to_string(count), fs::view("assets://shaders/color.shs"));
                newMat = rendering::MaterialCache::create_material("vertex color" + std::to_string(count), colorShader);
                newMat.set_param("color", math::color(math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f)));
            }

            mesh_filter meshFilter = mesh_filter(meshH);
            //mesh_filter(cubeH.get_mesh()),
            newEnt.add_components<rendering::mesh_renderable>(meshFilter, rendering::mesh_renderer(newMat));

            
            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(newEnt);
            positionH.write(posH.read());
            count++;

            ObjectToFollow followObj;
            followObj.ent = physicsEnt;
            newEnt.add_component(followObj);

            //newEnt.set_parent(physicsEnt);
            currentContainer.push_back(newEnt);

        }


      
    }



    void PhysicsTestSystem::drawPhysicsColliders()
    {
        static float offset = 0.005f;
        static auto physicsQuery = createQuery< physics::physicsComponent>();
        physicsQuery.queryEntities();

        for (auto entity : physicsQuery)
        {
            auto rotationHandle = entity.get_component_handle<rotation>();
            auto positionHandle = entity.get_component_handle<position>();
            auto scaleHandle = entity.get_component_handle<scale>();
            auto physicsComponentHandle = entity.get_component_handle<physics::physicsComponent>();

            bool hasTransform = rotationHandle && positionHandle && scaleHandle;
            bool hasNecessaryComponentsForPhysicsManifold = hasTransform && physicsComponentHandle;

            if (hasNecessaryComponentsForPhysicsManifold)
            {
                auto rbColor = math::color(0.0, 0.5, 0, 1);
                auto statibBlockColor = math::color(0, 1, 0, 1);

                rotation rot = rotationHandle.read();
                position pos = positionHandle.read();
                scale scale = scaleHandle.read();

                auto usedColor = rbColor;
                bool useDepth = false;

                if (entity.get_component_handle<physics::rigidbody>())
                {
                    usedColor = rbColor;
                    //useDepth = true;
                }


                //assemble the local transform matrix of the entity
                math::mat4 localTransform;
                math::compose(localTransform, scale, rot, pos);

                auto physicsComponent = physicsComponentHandle.read();

                for (auto physCollider : physicsComponent.colliders)
                {
                    
                    //--------------------------------- Draw Collider Outlines ---------------------------------------------//
                    if (!physCollider->shouldBeDrawn) { continue; }
                    //math::vec3 colliderCentroid = pos + math::vec3(localTransform * math::vec4(physCollider->GetLocalCentroid(), 0));
                    //debug::drawLine(colliderCentroid, colliderCentroid + math::vec3(0.0f,0.2f,0.0f), math::colors::cyan, 6.0f,0.0f,true);

                    for (auto face : physCollider->GetHalfEdgeFaces())
                    {
                       



                        //face->forEachEdge(drawFunc);
                        physics::HalfEdgeEdge* initialEdge = face->startEdge;
                        physics::HalfEdgeEdge* currentEdge = face->startEdge;
                        math::vec3 worldNormal = (localTransform * math::vec4(face->normal, 0));
                        math::vec3 faceStart = localTransform * math::vec4(face->centroid, 1);
                        math::vec3 faceEnd = faceStart + worldNormal * 0.1f;

                        auto camQuery = createQuery<rendering::camera,transform>();
                        camQuery.queryEntities();

                        math::vec3 camPos;
                        for (auto ent : camQuery)
                        {
                            camPos = ent.read_component<position>();
                        }

                        float dotResult = math::dot(camPos - faceStart, worldNormal);

                        if (dotResult < 0) { continue; }

                        //debug::drawLine(faceStart, faceEnd, math::colors::green, 2.0f);

                        if (!currentEdge) { return; }

                        do
                        {
                            physics::HalfEdgeEdge* edgeToExecuteOn = currentEdge;
                            currentEdge = currentEdge->nextEdge;
                            math::vec3 shift = worldNormal * offset;

                            math::vec3 worldStart = (localTransform * math::vec4(edgeToExecuteOn->edgePosition, 1)) ;
                            math::vec3 worldEnd = (localTransform * math::vec4(edgeToExecuteOn->nextEdge->edgePosition, 1)) ;

                            debug::drawLine(worldStart + shift, worldEnd + shift, usedColor, 2.0f, 0.0f, useDepth);

                            if (auto pairing = edgeToExecuteOn->pairingEdge)
                            {
                                //math::vec3 currentEdgeConnect = worldStart + shift + (worldEnd - worldStart + shift * 2.0f) * 0.25;
                                //math::vec3 currentMeet = worldStart + (worldEnd - worldStart) * 0.25;
                                ////debug::drawLine(currentEdgeConnect, currentMeet, math::colors::red, 5.0f, 0.0f, useDepth);

                                // math::vec3 pairingWorldStart = (localTransform * math::vec4(pairing->edgePosition, 1));
                                //math::vec3 pairinWorldEnd = (localTransform * math::vec4(pairing->nextEdge->edgePosition, 1));

                                //math::vec3 pairingMeet = pairingWorldStart + (pairinWorldEnd - pairingWorldStart) * 0.25;
                                //math::vec3 pairingEdgeConnect = pairingWorldStart + shift +
                                //    (pairinWorldEnd - pairingWorldStart + shift * 2.0f) * 0.25;

                                //debug::drawLine(pairingEdgeConnect, pairingMeet, math::colors::red, 5.0f, 0.0f, useDepth);
                            }

                        } while (initialEdge != currentEdge && currentEdge != nullptr);
                    }
                }

            }
        }
    }

    int step = 0;
    int maxStep = 0;

    void PhysicsTestSystem::quickHullStep(QHULL * action)
    {
        if (!action->value)
        {
            int i = 0;
            for (auto ent : registeredColliderColorDraw)
            {
                //[1] Get transform
                auto [posH,rotH,scaleH] = ent.get_component_handles<transform>();

                math::mat4 transform = math::compose(scaleH.read(), rotH.read(), posH.read());

                //auto 
                auto meshFilter = ent.read_component<mesh_filter>();

                //[1] clear colliders list
                auto physicsComponentH = ent.get_component_handle<physics::physicsComponent>();
                auto physComp = physicsComponentH.read();
                physComp.colliders.clear();
                physComp.ConstructConvexHull(meshFilter);
                physicsComponentH.write(physComp);

                //[4] use collider to generate follower objects
                //PopulateFollowerList(ent,i);
                i++;
            }

            step++;
            log::debug("PhysicsTestSystem::quickHullStep");
        }

    
      
    }

    void PhysicsTestSystem::AddRigidbodyToQuickhulls(AddRigidbody* action)
    {
        if (!action->value)
        {
            log::debug("Add body");
            for (auto ent : registeredColliderColorDraw)
            {
                ent.add_component<rigidbody>();
            }
        }


    }

    void PhysicsTestSystem::extendedContinuePhysics(extendedPhysicsContinue * action)
    {
        if (action->value)
        {
            physics::PhysicsSystem::IsPaused = false;
        }
        /*else
        {
            physics::PhysicsSystem::IsPaused = true;
        }*/

    }

    void PhysicsTestSystem::OneTimeContinuePhysics(nextPhysicsTimeStepContinue * action)
    {
        if (!(action->value))
        {
            physics::PhysicsSystem::IsPaused = true;
            physics::PhysicsSystem::oneTimeRunActive = true;
            log::debug(" onNextPhysicsTimeStepRequest");
        }
    }

    void PhysicsTestSystem::CreateElongatedFloor(math::vec3 position, math::quat rot, math::vec3 scale,rendering::material_handle mat
        ,bool hasCollider )
    {
        if (hasCollider)
        {
            cube_collider_params scaledCubeParams(scale.x, scale.z, scale.y);
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

        }
      
        ecs::entity_handle floor6;
        {
            floor6 = m_ecs->createEntity();

            floor6.add_components<rendering::mesh_renderable>
                (mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(mat));

            auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(floor6);
            positionH.write(position);
            rotationH.write(rot);
            scaleH.write(scale);

        }
    }

    void PhysicsTestSystem::createStack(int widthCount, int breadthCount, int heightCount, math::vec3 firstBlockPos, math::vec3 offset,
        rendering::model_handle cubeH, rendering::material_handle materials, physics::cube_collider_params cubeParams, bool useQuickhull, bool rigidbody , float mass , math::mat3 inverseInertia )
    {
        for (size_t y = 0; y < heightCount; y++)
        {
            for (size_t x = 0; x < widthCount; x++)
            {
                for (size_t z = 0; z < breadthCount; z++)
                {
                    math::vec3 finalPos = firstBlockPos + math::vec3(offset.x * x, offset.y * y, offset.z * z);
                    createBoxEntity(finalPos, cubeH, materials, cubeParams,useQuickhull,rigidbody,mass,inverseInertia);
                }
            }
        }
    }

    void PhysicsTestSystem::createBoxEntity(math::vec3 position, rendering::model_handle cubeH,
        rendering::material_handle materials, physics::cube_collider_params cubeParams, bool useQuickhull, bool rigidbody , float mass , math::mat3 inverseInertia )
    {
        auto ent = m_ecs->createEntity();

        if (rigidbody)
        {
            auto rbH = ent.add_component<physics::rigidbody>();
            auto rb = rbH.read();
            rb.setMass(mass);
            rb.localInverseInertiaTensor = inverseInertia;

            rbH.write(rb);
        }

        ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(materials));

        physics::physicsComponent physicsComponent;
        auto entPhyHande = ent.add_component<physics::physicsComponent>();

        if (useQuickhull)
        {
            physicsComponent.ConstructConvexHull(cubeH.get_mesh());
        }
        else
        {
            physicsComponent.AddBox(cubeParams);
        }

        entPhyHande.write(physicsComponent);

        

        auto [positionH, rotationH, scaleH] = m_ecs->createComponents<transform>(ent);
        positionH.write(position);
    }

}


