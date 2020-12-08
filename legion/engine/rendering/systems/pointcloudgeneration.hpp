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

    /**@class PointCloudGeneration
     * @brief A system that iterates all queried entities containing point_cloud and generates a particle system for them.
     */
    class PointCloudGeneration : public System<PointCloudGeneration>
    {
    public:

        /**@brief Setup inits the compute shader to sample the point clouds, creates update and does one initial generation.
          */
        void setup()
        {
            InitComputeShader();

            createProcess<&PointCloudGeneration::Update>("Update");

            Generate();
        }
        /**@Brief Every Update call Generate to check if new objects need to be generated
         */
        void Update(time::span deltaTime)
        {
            Generate();
        }

    private:
        //hard coded seed for now
        const int seed = 0;
        //index to index the particle system name
        int cloudGenerationCount = 0;
        //query containing point clouds
        ecs::EntityQuery query = createQuery<point_cloud>();
        //compute shader
        compute::function pointCloudGeneratorCS;

        void InitComputeShader()
        {
            //log::debug("init compute shader");
             if(!pointCloudGeneratorCS.isValid())
                pointCloudGeneratorCS = fs::view("assets://kernels/pointRasterizer.cl").load_as<compute::function>("Main");
        }
        //query entities and iterate them
        void Generate()
        {
           
            // log::debug("generating clouds");
            query.queryEntities();
            for (auto& ent : query)
            {
                GeneratePointCloud(ent.get_component_handle<point_cloud>());
            }
        }
        //generates point clouds
        void GeneratePointCloud(ecs::component_handle<point_cloud> pointCloud)
        {
            using compute::in, compute::out, compute::karg;

            auto realPointCloud = pointCloud.read();
            //exit early if point cloud has already been generated
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
                karg(realPointCloud.m_samplesPerTriangle,"samplePerTri"),
                out(result, "points")
            );
            //check if result is valid
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
            //log::debug("generating particle system");
            //generate particle system
            std::string name = "GeneratedPointCloud " + std::to_string(cloudGenerationCount);

            auto newPointCloud = ParticleSystemCache::createParticleSystem<PointCloudParticleSystem>(name, params, input);

            //create entity to store particle system
            auto newEnt = createEntity();
            newEnt.add_components<transform>(trans.get<position>().read(), trans.get<rotation>().read(), trans.get<scale>().read());

            rendering::particle_emitter emitter = newEnt.add_component<rendering::particle_emitter>().read();
            emitter.particleSystemHandle = newPointCloud;
            newEnt.get_component_handle<rendering::particle_emitter>().write(emitter);

            //increment index
            cloudGenerationCount++;
        }
    };



}

