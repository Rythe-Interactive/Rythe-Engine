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
#include <rendering/components/lod.hpp>
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
            if (!pointCloudGeneratorCS.isValid())
                pointCloudGeneratorCS = fs::view("assets://kernels/pointRasterizer.cl").load_as<compute::function>("Main");
        }
        //query entities and iterate them
        void Generate()
        {
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

            math::vec3 posiitonOffset = pointCloud.entity.get_component_handle<position>().read();
            //get mesh data
            auto m = realPointCloud.m_mesh.get();
            auto vertices = m.second.vertices;
            auto indices = m.second.indices;
            auto uvs = m.second.uvs;

            size_t triangle_count = indices.size() / 3;
            int divider = 2;
            uint process_Size = triangle_count;

            if (triangle_count > 10000)
            {
                while (triangle_count > 100000)
                {
                    triangle_count /= 2;
                    divider *= 2;
                }
                process_Size /= divider;

            }
            size_t points_Generated = (process_Size * realPointCloud.m_samplesPerTriangle);
            log::debug(points_Generated);
            //Generate points 
            std::vector<math::vec4> result(points_Generated);

            //Get normal map
            auto [lock, img] = realPointCloud.m_heightMap.get_raw_image();
            {
                async::readonly_guard guard(lock);
                auto normalMapBuffer = compute::Context::createImage(img, compute::buffer_type::READ_BUFFER, "normalMap");
                auto vertexBuffer = compute::Context::createBuffer(vertices, compute::buffer_type::READ_BUFFER, "vertices");
                auto indexBuffer = compute::Context::createBuffer(indices, compute::buffer_type::READ_BUFFER, "indices");
                auto uvBuffer = compute::Context::createBuffer(uvs, compute::buffer_type::READ_BUFFER, "uvs");
                auto outBuffer = compute::Context::createBuffer(result, compute::buffer_type::WRITE_BUFFER, "points");
                uint size = realPointCloud.m_heightMap.size().x;
                auto computeResult = pointCloudGeneratorCS
                (
                    process_Size,
                    vertexBuffer,
                    indexBuffer,
                    uvBuffer,
                    normalMapBuffer,
                    karg(realPointCloud.m_samplesPerTriangle, "samplePerTri"),
                    karg(realPointCloud.m_sampleDepth, "sampleWidth"),
                    karg(realPointCloud.m_heightStrength, "normalStrength"),
                    karg(size, "textureSize"),
                    outBuffer
                );
            }
            //translate vec4 into vec3
            std::vector<math::vec3> particleInput(points_Generated);
            for (int i = 0; i < points_Generated; i++)
            {
                particleInput.at(i) = result.at(i).xyz + posiitonOffset;
            }
            //generate particle params
            pointCloudParameters params
            {
               math::vec3(realPointCloud.m_pointRadius),
               realPointCloud.m_Material,
               ModelCache::get_handle("billboard")
            };
            GenerateParticles(params, particleInput, realPointCloud.m_trans);


            //write that pc has been generated
            realPointCloud.m_hasBeenGenerated = true;
            pointCloud.write(realPointCloud);
        }

        void GenerateParticles(pointCloudParameters params, std::vector<math::vec3> input, transform trans)
        {
            //generate particle system
            std::string name = "GeneratedPointCloud " + std::to_string(cloudGenerationCount);

            auto newPointCloud = ParticleSystemCache::createParticleSystem<PointCloudParticleSystem>(name, params, input);

            //create entity to store particle system
            auto newEnt = createEntity();

            //  newEnt.add_component <rendering::lod>();
            newEnt.add_components<transform>(trans.get<position>().read(), trans.get<rotation>().read(), trans.get<scale>().read());

            rendering::particle_emitter emitter = newEnt.add_component<rendering::particle_emitter>().read();
            emitter.particleSystemHandle = newPointCloud;
            newEnt.get_component_handle<rendering::particle_emitter>().write(emitter);

            //increment index
            cloudGenerationCount++;
        }
    };
}

