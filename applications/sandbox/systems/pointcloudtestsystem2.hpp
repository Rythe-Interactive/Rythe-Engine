#pragma once
#include<core/core.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>
#include <application/application.hpp>

#include <core/compute/context.hpp>
#include <core/compute/kernel.hpp>


#include <rendering/components/particle_emitter.hpp>

#include <rendering/systems/pointcloudgeneration.hpp>
using namespace legion;
using namespace rendering;
//system to test the point cloud generation system and the point cloud component
class pointcloudtestsystem2 final : public legion::core::System<pointcloudtestsystem2 >
{
public:
    ecs::entity_handle player;

    struct player_move : public app::input_axis<player_move> {};
    struct player_strive : public app::input_axis<player_strive> {};
    struct player_fly : public app::input_axis<player_fly> {};
    struct player_look_x : public app::input_axis<player_look_x> {};
    struct player_look_y : public app::input_axis<player_look_y> {};

    pointcloudtestsystem2()
    {
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(800, 600), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1); // Create the request for the main window.
    }


    virtual void setup() override
    {

        auto ent = createEntity();
        auto trans = ent.add_components<transform>(position(-5, 0, 0), rotation(), scale(0.5f));
        //get mesh
        ModelCache::create_model("cube", "assets://models/Cube.obj"_view);
        mesh_handle mesh = MeshCache::get_handle("cube");
        //create particle system material
        material_handle newMat;
        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);
            auto colorshader = rendering::ShaderCache::create_shader("color", "assets://shaders/color.shs"_view);
            newMat = rendering::MaterialCache::create_material("directional light", colorshader);
            newMat.set_param("color", math::colors::black);
        }
        ent.add_component<point_cloud>(point_cloud(mesh, trans, newMat, 150, 0.2f));
    }
};





