#include<core/core.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>

#include <core/compute/context.hpp>
#include <core/compute/kernel.hpp>
using namespace legion;
using namespace rendering;


class PointCloudTestSystem final : public legion::core::System<PointCloudTestSystem>
{
public:
    ecs::entity_handle player;
    rendering::model_handle cubeH;
    rendering::material_handle cubeMat;
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

        //log::debug("loading model");
        //ModelCache::create_model("cube", "assets://models//Cube.obj"_view);
        //log::debug("done model");

        ////  log::debug("loading shader");
        ////  auto colorShader = ShaderCache::create_shader("color", "assets://color.shs"_view);
        //log::debug("creating material");

        //cubeMat = MaterialCache::create_material("colorMat", "assets://color.shs"_view);
        //cubeMat.set_param("color", math::colors::yellow);

        //log::debug("creating entity");
        //auto ent = createEntity();
        //ent.add_component<renderable>({ ModelCache::get_handle("cube"),cubeMat });
        //ent.add_components<transform>();


        ModelCache::create_model("cube", "assets://models/cube.obj"_view);
        auto colorMat = MaterialCache::create_material("colorMat", "assets://shaders/color.shs"_view);
        colorMat.set_param("color", math::colors::yellow);

        auto ent = createEntity();
        ent.add_component<renderable>({ ModelCache::get_handle("cube"), colorMat });
        ent.add_components<transform>();
        //        auto result = fs::view("assets://models/Cube.obj").get();
        /*   auto result = fs::view("assets://models/Cube.obj").load_as<>

           if (result == common::valid)
           {
               auto resource = result.decay();

           }*/
    }
};





