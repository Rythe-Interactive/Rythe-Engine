#pragma once
#include <core/core.hpp>
#include <physics/halfedgeedge.hpp>
#include <application/application.hpp>
#include <core/math/math.hpp>

#include <core/logging/logging.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/components/rigidbody.hpp>
#include <physics/cube_collider_params.hpp>
#include <physics/data/physics_manifold_precursor.h>
#include <physics/systems/physicssystem.hpp>
#include <physics/halfedgeface.hpp>

#include <core/compute/context.hpp>
#include <core/compute/kernel.hpp>
#include <rendering/debugrendering.hpp>

using namespace args;

struct sah
{
    int value;

    sah operator+(const sah& other)
    {
        return { value + other.value };
    }

    sah operator*(const sah& other)
    {
        return { value * other.value };
    }
};

struct physicsIdentifier
{

};

struct player_move : public app::input_axis<player_move> {};
struct player_strive : public app::input_axis<player_strive> {};
struct player_fly : public app::input_axis<player_fly> {};
struct player_look_x : public app::input_axis<player_look_x> {};
struct player_look_y : public app::input_axis<player_look_y> {};

struct exit_action : public app::input_action<exit_action> {};

struct fullscreen_action : public app::input_action<fullscreen_action> {};
struct escape_cursor_action : public app::input_action<escape_cursor_action> {};
struct vsync_action : public app::input_action<vsync_action> {};

class TestSystem final : public System<TestSystem>
{
public:
    ecs::entity_handle player;

    virtual void setup()
    {
        filter(log::severity::debug);

        compute::Program prog = fs::view("basic://kernels/vadd_kernel.cl").load_as<compute::Program>();
        prog.prewarm("vector_add");

        std::vector<int> ints;

        auto res = fs::view("basic://bigint.txt").get();
        if (res == common::valid) {

            char* buf = new char[6];
            memset(buf, 0, 6);
            filesystem::basic_resource contents = res;

            for (size_t i = 0; i < contents.size() && i < 5 * 2048; i += 5)
            {
                memcpy(buf, contents.data() + i, 5);
                ints.push_back(std::atol(buf));
            }

            delete[] buf;
        }

        std::vector<int> first_ints(ints.begin(), ints.begin() + ints.size() / 2);
        std::vector<int> second_ints(ints.begin() + ints.size() / 2, ints.end());

        size_t to_process = std::min(first_ints.size(), second_ints.size());

        std::vector<int> results(to_process);


        auto A = compute::Context::createBuffer(first_ints, compute::buffer_type::READ_BUFFER, "A");
        auto B = compute::Context::createBuffer(second_ints, compute::buffer_type::READ_BUFFER, "B");
        auto C = compute::Context::createBuffer(results, compute::buffer_type::WRITE_BUFFER, "C");

        prog.kernelContext("vector_add")
            .set_and_enqueue_buffer(A)
            .set_and_enqueue_buffer(B)
            .set_buffer(C)
            .global(1024)
            .local(64)
            .dispatch()
            .enqueue_buffer(C)
            .finish();

        /* for (int& i : results)
         {
             log::info("got {}", i);
         }*/

        log::info("Hello World");
        log::warn("Hello World");
        log::error("Hello World");
        log::debug("Hello World");

        app::InputSystem::createBinding<player_move>(app::inputmap::method::W, 1.f);
        app::InputSystem::createBinding<player_move>(app::inputmap::method::S, -1.f);
        app::InputSystem::createBinding<player_strive>(app::inputmap::method::D, 1.f);
        app::InputSystem::createBinding<player_strive>(app::inputmap::method::A, -1.f);
        app::InputSystem::createBinding<player_fly>(app::inputmap::method::SPACE, 1.f);
        app::InputSystem::createBinding<player_fly>(app::inputmap::method::LEFT_SHIFT, -1.f);
        app::InputSystem::createBinding<player_look_x>(app::inputmap::method::MOUSE_X, 0.f);
        app::InputSystem::createBinding<player_look_y>(app::inputmap::method::MOUSE_Y, 0.f);
        app::InputSystem::createBinding<exit_action>(app::inputmap::method::ESCAPE);
        app::InputSystem::createBinding<fullscreen_action>(app::inputmap::method::F11);
        app::InputSystem::createBinding<escape_cursor_action>(app::inputmap::method::TAB);
        app::InputSystem::createBinding<vsync_action>(app::inputmap::method::F1);

        bindToEvent<player_move, &TestSystem::onPlayerMove>();
        bindToEvent<player_strive, &TestSystem::onPlayerStrive>();
        bindToEvent<player_fly, &TestSystem::onPlayerFly>();
        bindToEvent<player_look_x, &TestSystem::onPlayerLookX>();
        bindToEvent<player_look_y, &TestSystem::onPlayerLookY>();
        bindToEvent<exit_action, &TestSystem::onExit>();
        bindToEvent<fullscreen_action, &TestSystem::onFullscreen>();
        bindToEvent<escape_cursor_action, &TestSystem::onEscapeCursor>();
        bindToEvent<vsync_action, &TestSystem::onVSYNCSwap>();

        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        window.enableCursor(false);
        window.show();

        rendering::model_handle cubeH;
        rendering::model_handle sphereH;
        rendering::model_handle suzanneH;
        //rendering::model_handle gnomeH;
        rendering::model_handle uvsphereH;
        rendering::model_handle axesH;
        rendering::model_handle submeshtestH;
        rendering::model_handle floorH;

        rendering::material_handle wireframeH;
        rendering::material_handle vertexH;

        rendering::material_handle uvH;
        rendering::material_handle normalH;
        rendering::material_handle skyboxH;
        rendering::material_handle floorMH;

        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);

            cubeH = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            sphereH = rendering::ModelCache::create_model("sphere", "assets://models/sphere.obj"_view);
            suzanneH = rendering::ModelCache::create_model("suzanne", "assets://models/suzanne.obj"_view);
            //gnomeH = rendering::ModelCache::create_model("gnome", "assets://models/wizardgnome.obj"_view);
            uvsphereH = rendering::ModelCache::create_model("uvsphere", "assets://models/uvsphere.obj"_view);
            axesH = rendering::ModelCache::create_model("axes", "assets://models/xyz.obj"_view, { true, false, "assets://models/xyz.mtl"_view });
            submeshtestH = rendering::ModelCache::create_model("submeshtest", "assets://models/submeshtest.obj"_view);
            floorH = rendering::ModelCache::create_model("floor", "assets://models/groundplane.obj"_view);

            wireframeH = rendering::MaterialCache::create_material("wireframe", "assets://shaders/wireframe.glsl"_view);
            vertexH = rendering::MaterialCache::create_material("vertex", "assets://shaders/position.glsl"_view);
            uvH = rendering::MaterialCache::create_material("uv", "assets://shaders/uv.glsl"_view);
            normalH = rendering::MaterialCache::create_material("normal", "assets://shaders/normal.glsl"_view);
            skyboxH = rendering::MaterialCache::create_material("skybox", "assets://shaders/skybox.glsl"_view);
            floorMH = rendering::MaterialCache::create_material("floor", "assets://shaders/groundplane.glsl"_view);

            app::ContextHelper::makeContextCurrent(nullptr);
        }

        {
            auto ent = createEntity();
            ent.add_component<rendering::renderable>({ cubeH, skyboxH });
            ent.add_components<transform>(position(), rotation(), scale(500.f));
            log::debug("has transform: {}", ent.has_components<transform>());
        }

        {
            auto ent = createEntity();
            ent.add_component<rendering::renderable>({ floorH, floorMH });
            ent.add_components<transform>();
        }

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ suzanneH, vertexH }, {});
            ent.add_components<transform>(position(0, 3, 5.1f), rotation(), scale());

            auto [positionH, rotationH, scaleH] = ent.get_component_handles<transform>();

            log::debug("p {}, r {}, s {}, has {}", positionH.read(), rotationH.read(), scaleH.read(), ent.has_components<transform>());

            ent.remove_components<transform>();

            log::debug("p {}, r {}, s {}, has {}", positionH.read(), rotationH.read(), scaleH.read(), ent.has_components<position, rotation, scale>());
            transform transf = ent.add_components<transform>(position(0, 3, 5.1f), rotation(), scale());

            auto& [_, rotationH2, scaleH2] = transf.handles;
            auto positionH2 = transf.get<position>();

            log::debug("p {}, r {}, s {}, has {}", positionH2.read(), rotationH2.read(), scaleH2.read(), ent.has_components<position, rotation, scale>());
        }

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ suzanneH, wireframeH }, {});
            ent.add_components<transform>(position(0, 3, 8.1f), rotation(), scale());
        }

        {
            auto ent = m_ecs->createEntity();
            ent.add_components<rendering::renderable, sah>({ suzanneH, normalH }, {});

            ent.add_components<transform>(position(0, 3, 11.1f), rotation(), scale());
        }

        /*   {
               auto ent = m_ecs->createEntity();
               ent.add_component<sah>();
               m_ecs->createComponent<rendering::renderable>(ent, { gnomeH, normalH });

               auto [positionH, rotationH, scaleH] = m_ecs->createComponent<transform>(ent);
               positionH.write(math::vec3(0, 2, 15.1f));
               scaleH.write(math::vec3(1.f));
           }*/

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ submeshtestH, normalH }, {});
            ent.add_components<transform>(position(0, 10, 0), rotation(), scale());
        }

        {
            auto ent = createEntity();
            ent.add_component<rendering::renderable>({ axesH, normalH });
            ent.add_components<transform>();
        }

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ cubeH, uvH }, {});
            ent.add_components<transform>(position(5.1f, 3, 0), rotation(), scale(0.75f));
        }

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ sphereH, normalH }, {});
            ent.add_components<transform>(position(0, 3, -5.1f), rotation(), scale(2.5f));
        }

        {
            auto ent = createEntity();
            ent.add_components<rendering::renderable, sah>({ uvsphereH, wireframeH }, {});
            ent.add_components<transform>(position(-5.1f, 3, 0), rotation(), scale(2.5f));
        }

        setupCameraEntity();

        //------------------------------------- Setup entity with rigidbody -------------------------------------------//

        auto physicsEnt = createEntity();

        //setup rendering for physics ent
        physicsEnt.add_component<rendering::renderable>({ cubeH, wireframeH });

        position bodyP(0, 4, 5.1f);
        physicsEnt.add_components<transform>(bodyP, rotation(), scale());

        //setup physics component on physics ent
        physics::physicsComponent physicsComponent;
        physics::physicsComponent::init(physicsComponent);

        physics::cube_collider_params cubeParams;
        physicsComponent.AddBox(cubeParams);

        physics::rigidbody rb;
        rb.globalCentreOfMass = bodyP;
        //rb.addForce( math::vec3(-9, 0, 0));
        rb.addForceAt(math::vec3(0, 4.5, 5.1f), math::vec3(-100, 0, 0));
        rb.globalCentreOfMass = bodyP;

        physicsEnt.add_components<physics::rigidbody, physics::physicsComponent>(rb, physicsComponent);

        {
            auto ent = createEntity();
            ent.add_component<physics::physicsComponent>();
            ent.add_component<rendering::renderable>({ cubeH, wireframeH });

            ent.add_components<transform>(position(5.1f, -2.0f, 0), rotation(), scale(0.25f));
        }


        createProcess<&TestSystem::update>("Update");
        createProcess<&TestSystem::differentThread>("TestChain");
        createProcess<&TestSystem::differentInterval>("TestChain", 1.f);

    }

    void setupCameraEntity()
    {
        player = createEntity();
        rotation rot = math::conjugate(math::normalize(math::toQuat(math::lookAt(math::vec3(0, 0, 0), math::vec3(0, 0, 1), math::vec3(0, 1, 0)))));
        player.add_components<transform>(position(0.f, 3.f, 0.f), rot, scale());

        rendering::camera cam;
        cam.set_projection(90.f, 0.1f, 1000.f);
        player.add_component(cam);
    }

    void onExit(exit_action* action)
    {
        if (action->released())
            raiseEvent<events::exit>();
    }

    void onFullscreen(fullscreen_action* action)
    {
        if (action->released())
        {
            raiseEvent<app::window_toggle_fullscreen_request>(world_entity_id, math::ivec2(100, 100), math::ivec2(1360, 768));
        }
    }

    void onEscapeCursor(escape_cursor_action* action)
    {
        if (action->released())
        {
            static bool enabled = false;
            app::window window = m_ecs->world.get_component_handle<app::window>().read();
            enabled = !enabled;
            window.enableCursor(enabled);
            window.show();
        }
    }

    void onVSYNCSwap(vsync_action* action)
    {
        if (action->released())
        {
            auto handle = m_ecs->world.get_component_handle<app::window>();
            app::window window = handle.read();
            window.setSwapInterval(window.swapInterval() ? 0 : 1);
            log::debug("set swap interval to {}", window.swapInterval());
            handle.write(window);
        }
    }

    void onPlayerMove(player_move* action)
    {
        auto posH = player.get_component_handle<position>();
        auto rot = player.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(0.f, 0.f, 1.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
    }

    void onPlayerStrive(player_strive* action)
    {
        auto posH = player.get_component_handle<position>();
        auto rot = player.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(1.f, 0.f, 0.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
    }

    void onPlayerFly(player_fly* action)
    {
        auto posH = player.get_component_handle<position>();
        posH.fetch_add(math::vec3(0.f, action->value * action->input_delta * 10.f, 0.f));
    }

    void onPlayerLookX(player_look_x* action)
    {
        auto rotH = player.get_component_handle<rotation>();
        rotH.fetch_multiply(math::angleAxis(action->value, math::vec3(0, 1, 0)));
        rotH.read_modify_write(rotation(), [](const rotation& src, rotation&& dummy)
            {
                (void)dummy;
                math::vec3 fwd = math::toMat3(src) * math::vec3(0.f, 0.f, 1.f);
                if (fwd.y < -0.95f)
                    fwd.y = -0.95f;
                else if (fwd.y > 0.95f)
                    fwd.y = 0.95f;
                fwd = math::normalize(fwd);
                math::vec3 right = math::cross(fwd, math::vec3(0.f, 1.f, 0.f));
                return (rotation)math::conjugate(math::toQuat(math::lookAt(math::vec3(0.f, 0.f, 0.f), fwd, math::cross(right, fwd))));
            });
    }

    void onPlayerLookY(player_look_y* action)
    {
        auto rotH = player.get_component_handle<rotation>();
        rotH.fetch_multiply(math::angleAxis(action->value, math::vec3(1, 0, 0)));
        rotH.read_modify_write(rotation(), [](const rotation& src, rotation&& dummy)
            {
                (void)dummy;
                math::vec3 fwd = math::toMat3(src) * math::vec3(0.f, 0.f, 1.f);
                if (fwd.y < -0.95f)
                    fwd.y = -0.95f;
                else if (fwd.y > 0.95f)
                    fwd.y = 0.95f;
                fwd = math::normalize(fwd);
                math::vec3 right = math::cross(fwd, math::vec3(0.f, 1.f, 0.f));
                return (rotation)math::conjugate(math::toQuat(math::lookAt(math::vec3(0.f, 0.f, 0.f), fwd, math::cross(right, fwd))));
            });
    }

    void update(time::span deltaTime)
    {
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(1, 0, 0), math::colors::red, 10);
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(0, 1, 0), math::colors::green, 10);
        debug::drawLine(math::vec3(0, 0, 0), math::vec3(0, 0, 1), math::colors::blue, 10);

        //log::info("still alive! {}",deltaTime.seconds());
        static auto query = createQuery<sah>();

        //static time::span buffer;
        static int frameCount;
        //static time::span accumulated;

        //buffer += deltaTime;
        //accumulated += deltaTime;
        frameCount++;

        for (auto entity : query)
        {
            auto comp = entity.get_component_handle<sah>();

            auto rot = entity.read_component<rotation>();

            rot *= math::angleAxis(math::deg2rad(45.f * deltaTime), math::vec3(0, 1, 0));

            entity.write_component(rot);

            comp.write({ frameCount });
        }

        //if (buffer > 1.f)
        //{
        //    buffer -= 1.f;

        //    for (auto entity : query)
        //    {
        //        auto comp = entity.get_component_handle<sah>();
        //        std::cout << "component value: " << comp.read().value << std::endl;
        //    }

        //    std::cout << "Hi! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
        //}
    }

    void differentInterval(time::span deltaTime)
    {
        static time::span buffer;
        static int frameCount;
        static time::span accumulated;

        buffer += deltaTime;
        accumulated += deltaTime;
        frameCount++;

        math::vec2 v;
        v.x = 10;
        v.y = 20;

        if (buffer > 1.f)
        {
            buffer -= 1.f;
            //std::cout << "This is a fixed interval!! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
        }
    }

    void differentThread(time::span deltaTime)
    {
        static auto query = createQuery<sah>();

        static time::span buffer;
        static int frameCount;
        static time::span accumulated;

        buffer += deltaTime;
        accumulated += deltaTime;
        frameCount++;

        /* if (buffer > 1.f)
         {
             buffer -= 1.f;

             for (auto entity : query)
             {
                 auto comp = entity.get_component_handle<sah>();
                 std::cout << "component value on different thread: " << comp.read().value << std::endl;
             }

             std::cout << "This is a different thread!! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
         }*/

         //if (accumulated > 10.f)
         //{
         //	std::cout << "raising exit event" << std::endl;
         //	raiseEvent<events::exit>();
         //	//throw args_exception_msg("hehehe fuck you >:D");
         //}
    }
};
