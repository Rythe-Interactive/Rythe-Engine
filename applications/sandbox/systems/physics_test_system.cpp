#include "physics_test_system.hpp"
#include <physics/components/physics_component.hpp>
#include <rendering/debugrendering.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/rendering.hpp>
#include <physics/physics.hpp>
#include <random>

namespace legion::physics
{
    void PhysicsTestSystem::setup()
    {
        using namespace legion::core::fs::literals;

        app::window& win = ecs::world.get_component<app::window>();
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
            sun.add_component<rendering::mesh_renderable>(mesh_filter(directionalLightH.get_mesh()), rendering::mesh_renderer(directionalLightMH));
            sun.add_component<rendering::light>(rendering::light::directional(math::color(1, 1, 0.8f), 10.f));
            sun.add_component<transform>(position(10, 10, 10), rotation::lookat(math::vec3(1, 1, -1), math::vec3::zero), scale());
        }


        createProcess<&PhysicsTestSystem::colliderDraw>("Physics",0.02f);

        quickhullTestScene();
        //BoxStackScene();
        //stabilityComparisonScene();
        //monkeyStackScene();

    }

    void PhysicsTestSystem::colliderDraw(time::span dt)
    {
        //drawPhysicsColliders();

        ecs::filter<ObjectToFollow> objectToFollowQuery;

        for (auto ent : objectToFollowQuery)
        {
            auto& objToFollow = ent.get_component<ObjectToFollow>().get();

            auto [posH,rotH,scaleH] = ent.get_component<transform>();
            auto [posH2, rotH2, scaleH2] = objToFollow.ent.get_component<transform>();

            posH = posH2.get();
            rotH = rotH2.get();
            scaleH = scaleH2.get();
        }
    }

    void PhysicsTestSystem::ActivateSpawnRandomHull(SpawnHullActive& action)
    {
        if (!action.value)
        {
            m_throwingHullActivated = !m_throwingHullActivated;
            log::debug("Spawn Hull Active: {0}", m_throwingHullActivated);
        }
    }

    void PhysicsTestSystem::spawnRandomConvexHullOnCameraLocation(SpawnRandomHullOnCameraLoc& action)
    {
        if (!m_throwingHullActivated || action.value) { return; }

        //log::debug("spawnRandomConvexHullOnCamerLocation");

        //create entity
        auto ent = createEntity();

        //add a transform component
        auto [positionH, rotationH, scaleH] = ent.add_component<transform>();

        //get camera position and set transform to camera postiion 
        ecs::filter<rendering::camera, transform> cameraQuery;

        math::vec3 cameraDirection;

        for (auto ent : cameraQuery)
        {
            auto [positionCamH, rotationCamH, scaleCamH] = ent.get_component<transform>();
            cameraDirection = rotationCamH.get() * math::vec3(0, 0, 1);
            positionH = positionCamH.get() + cameraDirection * 2.5f;
            
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
        rigidbody& rb = ent.add_component<physics::rigidbody>().get();

        rb.setMass(2.5f);
        rb.localInverseInertiaTensor = math::mat3(3.0f);
        rb.velocity = cameraDirection * 14.0f;

        //add a physics component and run quickhull
        auto& newPhysicsComponent = ent.add_component<physics::physicsComponent>().get();
        newPhysicsComponent.constructConvexHullFromVertices(quickhullVertices);

        //using vertices of convex hull, create a rendering mesh out of it
        auto convexCollider = std::dynamic_pointer_cast<ConvexCollider>(newPhysicsComponent.colliders.at(0));

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


        static int procCount = 0;
        std::string name = std::string("QuickhullMesh") + std::to_string(procCount);

        procCount++;
        //TODO re enable the adding of the mesh renderer after the recursive template bug has been fixed
        //core::assets::asset<mesh> meshAsset = core::assets::AssetCache<mesh>::create(name,newMesh);
        //auto modelH = rendering::ModelCache::create_model(meshAsset);
        //count++;
        
        //create renderable
        //mesh_filter meshFilter = mesh_filter(modelH.get_mesh());

        //ent.add_component<rendering::mesh_renderable>(meshFilter, rendering::mesh_renderer(concreteH));
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
            folowerObjects.push_back(std::vector<ecs::entity>());
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

        auto ent = createEntity();

        auto [positionH, rotationH, scaleH] = ent.add_component<transform>();
        positionH = position;

        //ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(textureH));

        auto& entPhysicsComponent = ent.add_component<physics::physicsComponent>().get();
        entPhysicsComponent.AddBox(cubeParams);

        auto ent2 = createEntity();
        ent2.add_component<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(defaultStairMaterial));

        auto [position2H, rotation2H, scale2H] = ent2.add_component<transform>(); 
        position2H = position;
        scale2H = math::vec3(cubeParams.width, 1.0f, breadthMult);
    }

    void PhysicsTestSystem::createQuickhullTestObject(math::vec3 position, rendering::model_handle cubeH, rendering::material_handle TextureH, math::mat3 inertia )
    {
        physics::cube_collider_params cubeParams;
        cubeParams.breadth = 1.0f;
        cubeParams.width = 1.0f;
        cubeParams.height = 1.0f;

        auto ent = createEntity();

        auto [positionH, rotationH, scaleH] = ent.add_component<transform>(); 
        positionH = position;

        ent.add_component<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(TextureH));

        auto& entPhysicsComp = ent.add_component<physics::physicsComponent>().get();
        entPhysicsComp.constructConvexHullFromVertices(cubeH.get_mesh()->vertices);

        auto& rb = ent.add_component<rigidbody>().get();
        rb.localInverseInertiaTensor = inertia;

        registeredColliderColorDraw.push_back(ent);
    }

    void PhysicsTestSystem::PopulateFollowerList(ecs::entity physicsEnt, int index)
    {
        app::window& window = ecs::world.get_component<app::window>();

        auto& physicsComp = physicsEnt.get_component<physicsComponent>().get();
        auto collider = std::dynamic_pointer_cast<ConvexCollider>(physicsComp.colliders.at(0));
        auto [posH, rotH, scaleH] = physicsEnt.get_component<transform>();

        auto& currentContainer = folowerObjects.at(index);

        for (auto ent : currentContainer)
        {
            ent.destroy();
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

            //TODO re enable the adding of the mesh renderer after the recursive template bug has been fixed
            //static int count = 0;
            //core::assets::asset<mesh> meshAsset = core::assets::AssetCache<mesh>::create("PopulateFollowerList " + count,newMesh);
            //count++;

            //creaate modelH
            //auto modelH = rendering::ModelCache::create_model(meshAsset);

            auto newEnt = createEntity();

            rendering::material_handle newMat;
            {
               /* app::context_guard guard(window);
                auto colorShader = rendering::ShaderCache::create_shader("color" + std::to_string(count), fs::view("assets://shaders/color.shs"));
                newMat = rendering::MaterialCache::create_material("vertex color" + std::to_string(count), colorShader);
                newMat.set_param("color", math::color(math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f), math::linearRand(0.25f, 0.7f)));*/
            }

            //mesh_filter meshFilter = mesh_filter(modelH.get_mesh());
            //newEnt.add_component<rendering::mesh_renderable>(meshFilter, rendering::mesh_renderer(newMat));

            
            auto [positionH, rotationH, scaleH] = newEnt.add_component<transform>(); 
            positionH = posH;
            //count++;

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
        ecs::filter< physics::physicsComponent,transform> physicsQuery;

        for (auto entity : physicsQuery)
        {
            auto& rot = entity.get_component<rotation>().get();
            auto& pos = entity.get_component<position>().get();
            auto& scaleComp = entity.get_component<scale>().get();
            auto& physicsComponent = entity.get_component<physics::physicsComponent>().get();

            auto rbColor = math::color(0.0, 0.5, 0, 1);
            auto statibBlockColor = math::color(0, 1, 0, 1);

            auto usedColor = rbColor;
            bool useDepth = false;

            if (entity.get_component<physics::rigidbody>())
            {
                usedColor = rbColor;
                useDepth = true;
            }


            //assemble the local transform matrix of the entity
            math::mat4 localTransform;
            math::compose(localTransform, scaleComp, rot, pos);

            for (auto physCollider : physicsComponent.colliders)
            {
                //--------------------------------- Draw Collider Outlines ---------------------------------------------//
                if (!physCollider->shouldBeDrawn) { continue; }

                for (auto face : physCollider->GetHalfEdgeFaces())
                {
                    physics::HalfEdgeEdge* initialEdge = face->startEdge;
                    physics::HalfEdgeEdge* currentEdge = face->startEdge;
                    math::vec3 worldNormal = (localTransform * math::vec4(face->normal, 0));
                    math::vec3 faceStart = localTransform * math::vec4(face->centroid, 1);
                    math::vec3 faceEnd = faceStart + worldNormal * 0.1f;

                    ecs::filter<rendering::camera, transform> camQuery;

                    math::vec3 camPos;
                    for (auto ent : camQuery)
                    {
                        camPos = ent.get_component<position>();
                    }

                    float dotResult = math::dot(camPos - faceStart, worldNormal);

                    if (dotResult < 0) { continue; }

                    if (!currentEdge) { return; }

                    do
                    {
                        physics::HalfEdgeEdge* edgeToExecuteOn = currentEdge;
                        currentEdge = currentEdge->nextEdge;
                        math::vec3 shift = worldNormal * offset;

                        math::vec3 worldStart = (localTransform * math::vec4(edgeToExecuteOn->edgePosition, 1));
                        math::vec3 worldEnd = (localTransform * math::vec4(edgeToExecuteOn->nextEdge->edgePosition, 1));

                        debug::drawLine(worldStart + shift, worldEnd + shift, usedColor, 2.0f, 0.0f, useDepth);

                    } while (initialEdge != currentEdge && currentEdge != nullptr);
                }
            }
        }
    }

    int step = 0;
    int maxStep = 0;

    void PhysicsTestSystem::quickHullStep(QHULL& action)
    {
        if (!action.value)
        {
            int i = 0;
            for (ecs::entity ent : registeredColliderColorDraw)
            {
                //[1] Get transform
                transform trans = ent.get_component<transform>();

                math::mat4 entTransform = trans.to_world_matrix();
                
                //auto 
                auto meshFilter = ent.get_component<mesh_filter>();

                //[1] clear colliders list
                auto& physicsComponent = ent.get_component<physics::physicsComponent>().get();

                physicsComponent.colliders.clear();
                physicsComponent.constructConvexHullFromVertices(meshFilter.get().shared_mesh->vertices);

                //[4] use collider to generate follower objects
                //PopulateFollowerList(ent,i);
                i++;
            }

            step++;
            log::debug("PhysicsTestSystem::quickHullStep");
        }

    
      
    }

    void PhysicsTestSystem::AddRigidbodyToQuickhulls(AddRigidbody& action)
    {
        if (!action.value)
        {
            log::debug("Add body");
            for (auto ent : registeredColliderColorDraw)
            {
                ent.add_component<rigidbody>();
            }
        }


    }

    void PhysicsTestSystem::extendedContinuePhysics(extendedPhysicsContinue & action)
    {
        if (action.value)
        {
            physics::PhysicsSystem::IsPaused = false;
        }
        /*else
        {
            physics::PhysicsSystem::IsPaused = true;
        }*/

    }

    void PhysicsTestSystem::OneTimeContinuePhysics(nextPhysicsTimeStepContinue & action)
    {
        if (!(action.value))
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
            ecs::entity floor5;
            {
                floor5 = createEntity();

                auto& entPhysicsComp = floor5.add_component<physics::physicsComponent>().get();
                entPhysicsComp.AddBox(scaledCubeParams);

                //floor.add_components<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(woodTextureH));

                auto& idH = floor5.add_component<physics::identifier>().get();
                idH.id = "floor";


                auto [positionH, rotationH, scaleH] = floor5.add_component<transform>(); 
                positionH = position;
                rotationH = rot;
                scaleH = math::vec3(1.0f, 1.0f, 1.0f);
            }

        }
      
        ecs::entity floor6;
        {
            floor6 = createEntity();

            floor6.add_component<rendering::mesh_renderable>
                (mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(mat));

            auto [positionH, rotationH, scaleH] = floor6.add_component<transform>(); 
            positionH = position;
            rotationH = rot;
            scaleH = scale;

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
        auto ent = createEntity();

        if (rigidbody)
        {
            auto& rbH = ent.add_component<physics::rigidbody>().get();
            rbH.setMass(mass);
            rbH.localInverseInertiaTensor = inverseInertia;
        }

        ent.add_component<rendering::mesh_renderable>(mesh_filter(cubeH.get_mesh()), rendering::mesh_renderer(materials));

        auto& entPhysicsComponent = ent.add_component<physics::physicsComponent>().get();

        if (useQuickhull)
        {
            entPhysicsComponent.constructConvexHullFromVertices(cubeH.get_mesh()->vertices);
        }
        else
        {
            entPhysicsComponent.AddBox(cubeParams);
        }

        auto [positionH, rotationH, scaleH] = ent.add_component<transform>(); 
        positionH = position;
    }

}


