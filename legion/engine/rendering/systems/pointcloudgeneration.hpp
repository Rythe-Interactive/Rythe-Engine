#pragma once
#include<core/core.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>

#include <core/compute/context.hpp>
#include <core/compute/kernel.hpp>
#include <core/compute/high_level/function.hpp>

#include <rendering/systems/pointcloud_particlesystem.hpp>
#include <rendering/components/point_cloud.hpp>
#include <rendering/components/particle_emitter.hpp>
using namespace legion;


namespace legion::rendering
{
    class PointCloudGeneration : public System<PointCloudGeneration>

    {
    public:

        void setup()
        {
            InitComputeShader();

            createProcess<&PointCloudGeneration::Update>("Update");

            Generate();
        }

        void Update(time::span deltaTime)
        {
            Generate();
        }

    private:
        //hard coded seed for now
        const int seed = 0;
        int cloudGenerationCount = 0;
        ecs::EntityQuery query = createQuery<point_cloud>();
        compute::function pointCloudGeneratorCS;


        void InitComputeShader()
        {
            log::debug("init compute shader");
            pointCloudGeneratorCS = fs::view("assets://kernels/pointRasterizer.cl").load_as<compute::function>("Main");
        }
        void Generate()
        {
            // log::debug("generating clouds");
            query.queryEntities();
            for (auto& ent : query)
            {
                GeneratePointCloud(ent.get_component_handle<point_cloud>());
            }
        }
        void GeneratePointCloud(ecs::component_handle<point_cloud> pointCloud)
        {
            using compute::in, compute::out;

            auto realPointCloud = pointCloud.read();
            if (realPointCloud.m_hasBeenGenerated) return;
            // log::debug("new point cloud generation!");

             //get mesh data
            auto m = realPointCloud.m_mesh.get();
            auto vertices = m.second.vertices;
            auto indices = m.second.indices;
            //log::debug(vertices.size());
            //log::debug(indices.size());
            size_t triangle_count = indices.size() / 3;
            uint process_Size = triangle_count;
            size_t points_Generated = (triangle_count * realPointCloud.m_samplesPerTriangle);
            //log::debug(realPointCloud.m_samplesPerTriangle);
            //Generate points 
            std::vector<math::vec4> result(points_Generated);
            auto computeResult = pointCloudGeneratorCS
            (
                process_Size, in(vertices, "vertices"),
                in(indices, "indices"),
                compute::karg(realPointCloud.m_samplesPerTriangle,"samplePerTri"),
                out(result, "points")
            );

            if (computeResult.valid())
            {
                //translate vec4 into vec3
                std::vector<math::vec3> particleInput(points_Generated);
                for (int i = 0; i < points_Generated; i++)
                {
                    //log::debug(result.at(i));
                    particleInput.at(i) = result.at(i).xyz;
                }
                //generate particle params
                pointCloudParameters params
                {
                   math::vec3(realPointCloud.m_pointRadius),
                   realPointCloud.m_Material,
                   ModelCache::get_handle("cube")
                };
                GenerateParticles(params, particleInput, realPointCloud.m_trans);
            }
            else
            {
                log::debug("Point cloud generation failed");
            }


            //write that pc has been generated
            realPointCloud.m_hasBeenGenerated = true;
            pointCloud.write(realPointCloud);
        }

        void GenerateParticles(pointCloudParameters params, std::vector<math::vec3> input, transform trans)
        {
          //  log::debug("generating particle system");
            //generate particle system
            std::string name = "GeneratedPointCloud " + std::to_string(cloudGenerationCount);


            auto newPointCloud = ParticleSystemCache::createParticleSystem<PointCloudParticleSystem>(name, params, input);

            //create entity to store particle system
            auto newEnt = createEntity();
            newEnt.add_components<transform>(trans.get<position>().read(), trans.get<rotation>().read(), trans.get<scale>().read());

            rendering::particle_emitter emitter = newEnt.add_component<rendering::particle_emitter>().read();
            emitter.particleSystemHandle = newPointCloud;
            newEnt.get_component_handle<rendering::particle_emitter>().write(emitter);
            cloudGenerationCount++;
        }
    };



}

