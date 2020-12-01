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
    const int samplesPerTriangle = 10;
    const int seed = 1;

    PointCloudTestSystem()
    {
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(800, 600), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1); // Create the request for the main window.
    }
    // struct player_move : public app::input_action<player_move> {};


    virtual void setup() override
    {
        log::debug("Start Point Cloud Test System setup");
        srand(seed);
        using compute::in, compute::out;
        app::window window = m_ecs->world.get_component_handle<app::window>().read();
        log::debug("Done loading window");


        log::debug("Loading OpenCL kernel");

        // create opencl-"function"
        auto computeShader = fs::view("assets://kernels/pointRasterizer.cl").load_as<compute::function>("Main");

        log::debug("Done loading OpenCL kernel");

        log::debug("loading models");
        rendering::material_handle colorMat;
        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);
            ModelCache::create_model("cube", "assets://models/Cube.obj"_view);
          /*  ModelCache::create_model("sphere", "assets://models/sphere.obj"_view);
            ModelCache::create_model("suzanne", "assets://models/suzanne.obj"_view);*/
            ModelCache::create_model("uvsphere", "assets://models/uvsphere.obj"_view);

            
            
            colorMat = MaterialCache::create_material("colorMat", "assets://shaders/color.shs"_view);
            app::ContextHelper::makeContextCurrent(nullptr);
        }
        colorMat.set_param("color", math::colors::green);
        log::debug("done loading models");

        //get vertecies
        auto cache = MeshCache::get_handle("uvsphere");
        auto m = cache.get();
        auto verts = m.second.vertices;
        auto indices = m.second.indices;
        int vertexCount = verts.size();
        //Create some info 
        size_t triangle_count = indices.size() / 3;
        size_t process_Size = triangle_count;
        size_t points_Generated = (triangle_count * samplesPerTriangle);

        log::debug("mesh indices count: " + std::to_string(indices.size()));

        log::debug("vertex count " + std::to_string(vertexCount));

        std::vector<uint> seeds(process_Size);
        srand(seed);
        for (size_t i = 0; i < process_Size; i++)
        {
            //   seeds.at(i) = std::rand();
        }

        //Create buffers
        auto vertBuffer = compute::Context::createBuffer(verts, compute::buffer_type::READ_BUFFER, "vertices");
        auto indexBuffer = compute::Context::createBuffer(indices, compute::buffer_type::READ_BUFFER, "indices");
        auto seedBuffer = compute::Context::createBuffer(seeds, compute::buffer_type::READ_BUFFER, "seed");
        std::vector<math::vec4> result(points_Generated);
        auto resultBuffer = compute::Context::createBuffer(result, compute::buffer_type::WRITE_BUFFER, "points");
        std::vector<float> randVec(100);
        for (int i = 0; i < 100; i++)
        {
            randVec.at(i) =( std::rand() / (float)RAND_MAX);
          //  log::debug(std::to_string(randVec.at(i)));
        }
        auto randbuffer = compute::Context::createBuffer(result, compute::buffer_type::READ_BUFFER, "randBuffer");

        //  process_Size = 1;
       //execute
        auto code1 = computeShader(process_Size, vertBuffer, indexBuffer, seedBuffer, randbuffer, resultBuffer);
        log::info("samples per triangle:  " + std::to_string(samplesPerTriangle));

        log::info("generated " + std::to_string(points_Generated) + " sample points:");

        //generate cubes
        float scaling = 1.0f;
        float individualScale = 0.025f;
        math::vec3 startPos = math::vec3(0, 3, -7);
        std::vector<ecs::entity_handle> enteties(points_Generated);
        if (code1.valid())
        {
            for (size_t i = 0; i < points_Generated; i++)
            {
                math::vec3 currentPos = result.at(i).xyz * scaling + startPos;

                auto ent = createEntity();
                ent.add_component<renderable>({ ModelCache::get_handle("cube"), colorMat });
                ent.add_components<transform>(currentPos, rotation(), scale(individualScale));
                enteties.at(i) = ent;
               //   log::info(" got: " + std::to_string(result.at(i).x) + ", " + std::to_string(result.at(i).y));
             //      log::info(" got: " + std::to_string(result.at(i).w));

            }
        }
    }
};





