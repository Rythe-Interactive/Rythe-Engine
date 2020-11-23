#include<core/core.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>
#include <application/application.hpp>

#include <core/compute/context.hpp>
#include <core/compute/kernel.hpp>

using namespace legion;
using namespace rendering;


class PointCloudTestSystem final : public legion::core::System<PointCloudTestSystem>
{
public:
    ecs::entity_handle player;

    struct player_move : public app::input_axis<player_move> {};
    struct player_strive : public app::input_axis<player_strive> {};
    struct player_fly : public app::input_axis<player_fly> {};
    struct player_look_x : public app::input_axis<player_look_x> {};
    struct player_look_y : public app::input_axis<player_look_y> {};

    PointCloudTestSystem()
    {
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(1360, 768), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1); // Create the request for the main window.
    }
    // struct player_move : public app::input_action<player_move> {};


    virtual void setup() override
    {
        log::debug("Start Point Cloud Test System setup");

        using compute::in, compute::out;


        //create entity to store camera
        player = createEntity();
        player.add_components<transform>(position(0.f, 3.f, 0.f), rotation::lookat(math::vec3::zero, math::vec3::forward), scale());
        //Create cam && window
        rendering::camera cam;
        cam.set_projection(90.f, 0.1f, 1000.f);
        player.add_component<rendering::camera>(cam);
        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        window.enableCursor(true);
        window.show();
        log::debug("Done loading window");


        log::debug("Loading OpenCL kernel");

        // create opencl-"function"
        auto vector_add = fs::view("assets://kernels/pointRasterizer.cl").load_as<compute::function>("Main");

        log::debug("Done loading OpenCL kernel");


        log::debug("loading models");
        rendering::material_handle colorMat;
        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);
            ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            colorMat = MaterialCache::create_material("colorMat", "assets://shaders/color.shs"_view);
            app::ContextHelper::makeContextCurrent(nullptr);
        }
        colorMat.set_param("color", math::colors::yellow);
        log::debug("done loading models");


        auto ent = createEntity();
        ent.add_component<renderable>({ ModelCache::get_handle("cube"), colorMat });
        ent.add_components<transform>(position(-10, 0,0 ), rotation(), scale(500.f));


        app::InputSystem::createBinding<player_move>(app::inputmap::method::W, 1.f);
        app::InputSystem::createBinding<player_move>(app::inputmap::method::S, -1.f);
        app::InputSystem::createBinding<player_move>(app::inputmap::method::W, 1.f);
        app::InputSystem::createBinding<player_move>(app::inputmap::method::S, -1.f);
        app::InputSystem::createBinding<player_strive>(app::inputmap::method::D, 1.f);
        app::InputSystem::createBinding<player_strive>(app::inputmap::method::A, -1.f);
        app::InputSystem::createBinding<player_fly>(app::inputmap::method::SPACE, 1.f);
        app::InputSystem::createBinding<player_fly>(app::inputmap::method::LEFT_SHIFT, -1.f);
        app::InputSystem::createBinding<player_look_x>(app::inputmap::method::MOUSE_X, 0.f);
        app::InputSystem::createBinding<player_look_y>(app::inputmap::method::MOUSE_Y, 0.f);

        app::InputSystem::createBinding<player_hover>(app::inputmap::method::Q, 1.f);
        app::InputSystem::createBinding<player_hover>(app::inputmap::method::E, -1.f);

        bindToEvent<player_move, &PointCloudTestSystem::onPlayerMove>();
        bindToEvent<player_strive, &PointCloudTestSystem::onPlayerStrive>();
        //    bindToEvent<player_fly, &PointCloudTestSystem::onPlayerFly>();
        bindToEvent<player_look_x, &PointCloudTestSystem::onPlayerLookX>();
        bindToEvent<player_look_y, &PointCloudTestSystem::onPlayerLookY>();

        // app::InputSystem::createBinding<custimInpit>(app::inputmap::method::T);
         //bindToEvent<custimInpit, &TestSystem>
         //        auto result = fs::view("assets://models/Cube.obj").get();
         /*   auto result = fs::view("assets://models/Cube.obj").load_as<>

            if (result == common::valid)
            {
                auto resource = result.decay();

            }*/
    }

    void onPlayerMove(player_move* action)
    {
        auto posH = player.get_component_handle<position>();
        auto rot = player.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(0.f, 0.f, 1.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
        /*   log::debug("pos x" + std::to_string(posH.read().x));
           log::debug("pos y" + std::to_string(posH.read().y));
           log::debug("pos z" + std::to_string(posH.read().z));*/

    }
    void onPlayerStrive(player_strive* action)
    {
        auto posH = player.get_component_handle<position>();
        auto rot = player.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(1.f, 0.f, 0.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * 10.f;
        posH.fetch_add(move);
    }
    void onPlayerHover(player_fly* action)
    {
        auto posH = player.get_component_handle<position>();
        math::vec3 move = math::vec3(0, 1, 0) * action->value * 0.01f;
        posH.fetch_add(move);
    }
    void onPlayerFly(player_look_x* action) {}

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
};





