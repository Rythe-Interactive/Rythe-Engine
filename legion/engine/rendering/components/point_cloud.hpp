#pragma once
#include<core/core.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>
#include <application/application.hpp>

#include <core/compute/context.hpp>
#include <core/compute/kernel.hpp>


namespace legion::rendering
{
    /**@class point_cloud
     * @brief Struct that holds data for the mandatory for the pointcloud generation
     */
    struct point_cloud
    {
    public:

        /**@brief Takes in a mesh handle, transform and a material, optionally it takes in max points and radius.
         * @param newMesh is used to sample the points for the point cloud.
         * @param newTrans is the position of the object / point cloud.
         * @param mat is the material used by the particle system that renders the points.
         * @param maxPoints is used to calculate the samples for each triangle of the mesh,
         * @param radius defines point scale.
         */
        point_cloud(mesh_handle newMesh, transform newTrans, material_handle mat, uint maxPoints = 100, float radius = 0.2f)
        {
            m_mesh = newMesh;
            m_trans = newTrans;
            //m_samplesPerTriangle = calculateSamplesPerTriangle(maxPoints);
            m_pointRadius = radius;
            m_Material = mat;
            InitSampleValues(maxPoints);
        }
        point_cloud() = default;

    private:
        friend class PointCloudGeneration;
        //used to check if a new point cloud needs to be generated
        bool m_hasBeenGenerated = false;

        //point cloud parameters
        float m_pointRadius;
        transform m_trans;
        mesh_handle m_mesh;
        uint m_samplesPerTriangle;
        uint m_sampleDepth;
        material_handle m_Material;


        void InitSampleValues(uint maxPoints)
        {
            uint samplesPerTri = calculateSamplesPerTriangle(maxPoints);

            //generate sample width
            uint currentIterator = 0;
            uint sum = 0;
            uint previousSum = 0;
            //iterate until the samples reach the max sample per triangle
            // add more rows until the sum of all rows exceeds the sample per triangle count:
            // *
            // *  *
            // *  *  *
            // *  *  *  *
            while (sum < samplesPerTri)
            {
                previousSum = sum;
                sum = 0;
                currentIterator++;
                //exit if too many iterations are made 
                if (currentIterator > 30)
                {
                    log::error("please decrease max sample points for point cloud object");
                    return;
                }
                
                for (int i = currentIterator; i > 0; i--)
                {
                    sum += i;
                }
            }
            m_samplesPerTriangle = previousSum;
            m_sampleDepth = currentIterator - 1;
            log::debug("samples per tri " + std::to_string(m_samplesPerTriangle));
            log::debug("samples depth " + std::to_string(m_sampleDepth));

        }
        uint calculateSamplesPerTriangle(uint maxPoints)
        {
            if (!m_mesh) return 0;
            auto mesh = m_mesh.get();
            auto indices = mesh.second.indices;
            uint triangleCount = indices.size() / 3;
            return maxPoints / triangleCount;
        }

        uint calculateSampleWidth(uint samplePerTriangle)
        {


        }
    };
}
